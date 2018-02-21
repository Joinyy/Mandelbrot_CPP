#ifdef _WIN32
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#define _ENABLE_ATOMIC_ALIGNMENT_FIX
#endif

#include <thread>
#include <future>
#include <mutex>
#include <atomic>
#include <chrono>
#include <string>
#include <iostream>
#include <array>

#include "CarthCoords.h"
#include "helper_functions.hpp"
#include "mandelbrot.hpp"

unsigned int XW = 1920;
unsigned int YW = 1080;
unsigned int THREADCOUNT;
sf::RenderTexture mandelbrotTexture;

// Module local data:
sf::Texture texture;
sf::Sprite sprite;
sf::Text text;
std::atomic<bool> focus = true, debug = false, redraw_active = false, redraw = true;

std::string debugstring = "";
std::atomic<double> zoom = 3;
std::atomic<CarthCoords *> CooSys;

void renderingThread(sf::RenderWindow* window)
{
	if (!mandelbrotTexture.create(XW, YW))
	{
		std::cerr << "Could not create texture" << std::endl;
		window->close();
	}
	std::packaged_task<void(double x_cent, double y_cent, double zoom, double max_betrag, int xpixels, int ypixels, int max_iter)>* task = NULL;
	std::future<void> future;
	std::thread * a = NULL;
	using namespace std::chrono_literals;
	// the rendering loop
	while (window->isOpen())
	{
		// Worker für Mandelbrot starten
		if (redraw) {
			task = new std::packaged_task<void(double, double, double, double, int, int, int)>(mandelMain);
			future = task->get_future();
			a = new std::thread(std::move(*task), CooSys.load()->getCenter().x, CooSys.load()->getCenter().y, CooSys.load()->getZoom(), 20, XW, YW, 3000);
			redraw = false;
			redraw_active = true;
		}
		// worker für Mandelbrot abfragen
		if (redraw_active) {
			std::future_status status = future.wait_for(0ms);
			if (status == std::future_status::ready && a != NULL) {
				a->join();
				mandelbrotTexture.clear();
				for (std::size_t xxx = 0; xxx < XW; xxx++) {
					for (std::size_t yyy = 0; yyy < YW; yyy++) {
						mandelbrotTexture.draw(&arr[xxx][yyy], 1, sf::Points);
					}
				}
				mandelbrotTexture.display();
				texture = mandelbrotTexture.getTexture();
				sprite.setTexture(texture);
				redraw_active = false;
				debugstring.append("Redraw finished\n");
			}
		}


		// clear the window with black color
		window->clear(sf::Color::Black);

		// draw everything here...
		window->draw(sprite);
		// Debug Nachrichten
		if (debug) {
			sf::Vector2i position = sf::Mouse::getPosition(*window);
			sf::Vector2<double> capos = CooSys.load()->pixToCarth(position);
			std::string dadadubs;
			dadadubs = std::to_string(capos.x) + '|' + std::to_string(capos.y) + '\n';
			if (debugstring.length() > 400) {
				int firstNewline = debugstring.find_first_of('\n', 1);
				debugstring.erase(0, firstNewline + 1);
			}
			dadadubs.append(debugstring);
			text.setString(dadadubs);
			window->draw(text);
		}


		// end the current frame
		window->display();

		// end the current frame
		window->display();
	}
	delete task;
	delete a;
}

int main(int argc, char *argv[])
{
	// SFML Window bauen
	sf::ContextSettings settings;
	settings.antialiasingLevel = 2;
	settings.majorVersion = 0;
	settings.minorVersion = 1;
	
	sf::VideoMode Desktop;
	sf::RenderWindow window(Desktop.getDesktopMode(), "Mandelbrot ist lecker", sf::Style::Fullscreen, settings);
	sf::Vector2u size = window.getSize();
	arr = new sf::Vertex*[size.x];
	for (std::size_t i = 0; i < size.x; i++)
		arr[i] = new sf::Vertex[size.y];
	XW = size.x;
	YW = size.y;
	int ThrCnt = 8;
	while (YW % ThrCnt) {
		ThrCnt++;
		if (ThrCnt > 40) {
			std::cerr << "No valid threadcount found... What resolution are you using??!?!?!?" << std::endl;
			return 1;
		}
	}
	THREADCOUNT = ThrCnt;
	debugstring.append("HW Concurrency: " + std::to_string(std::thread::hardware_concurrency()) + '\n');
	debugstring.append("Threads used:   " + std::to_string(THREADCOUNT) + '\n');
	window.setFramerateLimit(30);

	// Color management
	std::array<std::string, 5> colormaps = { "jet.csv", "pink.csv", "parula.csv", "summer.csv", "gray.csv" };
	std::size_t colorCounter = 0;
	if (!loadColors(colormaps.at(colorCounter))) {
		std::cerr << "Could not load colors.";
		return 1;
	}
	// Teig für Mandelbrot anrühren
	CooSys = new CarthCoords(3.0, sf::Vector2<double>(0, 0), sf::Vector2<double>(1.5, 1.5), sf::Vector2i(XW, YW));
	sf::Font font;
	if (!font.loadFromFile("FiraMono-Regular.ttf"))
	{
		std::cerr << "Load font problem" << std::endl;
	}

	text.setFont(font);
	text.setCharacterSize(15);
	text.setFillColor(sf::Color::White);
	text.setOutlineColor(sf::Color::Color(20, 20, 20, 255));
	text.setOutlineThickness(3);
	text.setStyle(sf::Text::Regular);
	window.setActive(false);
	std::thread* renderThr = new std::thread(renderingThread, &window);
	
	// renderThr->detach();
	// Main Loop:
	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			// Window control Events:
			case sf::Event::Closed:
				window.close();
			case sf::Event::MouseLeft:
				focus = false;
				break;
			case sf::Event::MouseEntered:
				focus = true;
				break;
			// Mausrad input:
			case sf::Event::MouseWheelScrolled:
				if (focus && !redraw && !redraw_active) {
					if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
						if (event.mouseWheelScroll.delta > 0) {
							// Zoom in
							sf::Vector2i position = sf::Mouse::getPosition(window);
							CooSys.load()->zoomIn(position.x, position.y, 2.0);
							debugstring.append("Redraw at: X = " + std::to_string(CooSys.load()->getCenter().x) + " Y = " + std::to_string(CooSys.load()->getCenter().y) + " with Zoom: " + std::to_string(CooSys.load()->getZoom()) + "\n");
							redraw = true;
						}
						else {
							// Zoom out
							sf::Vector2i position = sf::Mouse::getPosition(window);
							CooSys.load()->zoomIn(position.x, position.y, 0.75);
							debugstring.append("Redraw at: X = " + std::to_string(CooSys.load()->getCenter().x) + " Y = " + std::to_string(CooSys.load()->getCenter().y) + " with Zoom: " + std::to_string(CooSys.load()->getZoom()) + "\n");
							redraw = true;
						}
					}
				}
				break;
			// Tastatur Input:
			case sf::Event::KeyPressed:
				if (focus) {
					switch (event.key.code)
					{
					case sf::Keyboard::Escape:
						window.close();
						break;
					case sf::Keyboard::D:
						debug = (debug) ? false : true;
						break;
					case sf::Keyboard::C:
						colorCounter++;
						if (colorCounter >= colormaps.size())
							colorCounter = 0;
						debugstring.append("Loading colormap: " + colormaps.at(colorCounter) + '\n');
						loadColors(colormaps.at(colorCounter));
						redraw = true;
						break;
					default:
						break;
					}
				}
			default:
				break;
			}
		}
		
	}
	renderThr->join();
	return 0;
}
