#ifdef _WIN32
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#define _ENABLE_ATOMIC_ALIGNMENT_FIX
#endif
#ifdef __linux__
#define _GLIBCXX_USE_CXX11_ABI 0
#include <X11/Xlib.h>
#endif

#include <thread>
#include <future>
#include <mutex>
#include <atomic>
#include <chrono>
#include <string>
#include <iostream>
#include <array>

#include "../hdr/CarthCoords.h"
#include "../hdr/helper_functions.hpp"
#include "../hdr/mandelbrot.hpp"

unsigned int XW = 1920;
unsigned int YW = 1080;
unsigned int THREADCOUNT;

// Module local data:
sf::Texture texture;
sf::Sprite sprite;
sf::Text text;
std::atomic<bool> focus(true), debug(false), showpos(false), redraw_active(false), redraw(true);

std::string debugstring = "";
std::atomic<double> zoom(3.0);
std::atomic<int> maxIterations(1000);
std::atomic<CarthCoords *> CooSys;

bool loadColors(std::string fileName) {
	bool retval = true;
	std::ifstream colorFile(fileName, std::ios::in);
	int lines = 0;
	int tokenCount = 0;

	if (colorFile.is_open()) {
		std::string line;

		while (std::getline(colorFile, line)) {
			std::istringstream ss(line);
			std::string token;
			while (std::getline(ss, token, ','))
			{
				switch (tokenCount) {
				case 0:
					ColorMap[lines].R = atoi(token.c_str());
					break;
				case 1:
					ColorMap[lines].G = atoi(token.c_str());
					break;
				case 2:
					ColorMap[lines].B = atoi(token.c_str());
					ColorMap[lines].A = 255;
					break;
				default:
					std::cerr << "Too many RGB Values per line..\n";
					break;
				}
				tokenCount++;
			}
			tokenCount = 0;
			lines++;
			if (lines > C_MAP_LEN) {
				std::cerr << "Too many lines in CSV.\n";
				break;
			}
		}
		colorFile.close();
	}
	else {
		std::cerr << "Unable to open file\n";
		retval = false;
	}
	return retval;
}

std::string to_string_prec(double a_value, int prec) {
	std::ostringstream out;
	out << std::setprecision(prec) << a_value;
	return out.str();
}


/* void renderingThread(sf::RenderWindow* window)
{
	if (!texture.create(XW, YW))
	{
		std::cerr << "Could not create texture" << std::endl;
		window->close();
	}
	std::packaged_task<void(double x_cent, double y_cent, double zoom, double max_betrag, int xpixels, int ypixels, int max_iter)>* task = NULL;
	std::future<void> future;
	std::thread * a = NULL;
	std::chrono::milliseconds t1(0);

	// Color management
	std::array<std::string, 5> colormaps = { "./res/jet.csv", "./res/pink.csv", "./res/parula.csv", "./res/summer.csv", "./res/gray.csv" };
	std::size_t colorCounter = 0;
	if (!loadColors(colormaps.at(colorCounter))) {
		std::cerr << "Could not load colors.";
	}
	std::array<int, 8> iterations = { 100, 500, 1000, 2000, 3000, 5000, 7500, 10000 };
	int itCount = 0;
	// the rendering loop
	while (window->isOpen()) 
	{
		// Worker für Mandelbrot starten
		if (redraw) {
			task = new std::packaged_task<void(double, double, double, double, int, int, int)>(mandelMain);
			future = task->get_future();
			a = new std::thread(std::move(*task), CooSys.load()->getCenter().x, CooSys.load()->getCenter().y, CooSys.load()->getZoom(), 20, XW, YW, maxIterations.load());
			redraw = false;
			redraw_active = true;
		}
		// worker für Mandelbrot abfragen
		if (redraw_active) {
			std::future_status status = future.wait_for(t1);
			if (status == std::future_status::ready && a != NULL) {
				a->join();
				texture.update(arr);
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
		if (debug || showpos) {
			sf::Vector2i position = sf::Mouse::getPosition(*window);
			sf::Vector2<double> capos = CooSys.load()->pixToCarth(position);
			std::string dadadubs;
			if (showpos) {
				dadadubs = to_string_prec(capos.x) + '|' + to_string_prec(capos.y) + '\n';
			}
			if (debugstring.length() > 500) {
				int firstNewline = debugstring.find_first_of('\n', 1);
				debugstring.erase(0, firstNewline + 1);
			}
			if (debug) {
				dadadubs.append(debugstring);
			}
			text.setString(dadadubs);
			window->draw(text);
		}


		// end the current frame
		window->display();
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window->pollEvent(event))
		{
			// std::cout << "Event has been detected!" << std::endl;
			switch (event.type)
			{
			// Window control Events:
			case sf::Event::Closed:
				window->close();
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
							sf::Vector2i position = sf::Mouse::getPosition(*window);
							CooSys.load()->zoomIn(position.x, position.y, 2.0);
							debugstring.append("Redraw at: X = " + to_string_prec(CooSys.load()->getCenter().x) + " Y = " + to_string_prec(CooSys.load()->getCenter().y) + " with Zoom: " + to_string_prec(CooSys.load()->getZoom()) + "\n");
							redraw = true;
						}
						else {
							// Zoom out
							sf::Vector2i position = sf::Mouse::getPosition(*window);
							CooSys.load()->zoomIn(position.x, position.y, 0.75);
							debugstring.append("Redraw at: X = " + to_string_prec(CooSys.load()->getCenter().x) + " Y = " + to_string_prec(CooSys.load()->getCenter().y) + " with Zoom: " + to_string_prec(CooSys.load()->getZoom()) + "\n");
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
						window->close();
						break;
					case sf::Keyboard::D:
						debug = (debug) ? false : true;
						break;
					case sf::Keyboard::P:
						showpos = (showpos) ? false : true;
						break;
					case sf::Keyboard::C:
						colorCounter++;
						if (colorCounter >= colormaps.size())
							colorCounter = 0;
						debugstring.append("Loading colormap: " + colormaps.at(colorCounter) + '\n');
						loadColors(colormaps.at(colorCounter));
						redraw = true;
						break;
					case sf::Keyboard::I:
						maxIterations = iterations.at(itCount);
						debugstring.append("Changing max iterations to " + std::to_string(iterations.at(itCount)) + '\n');
						itCount++;
						if (itCount >= iterations.size()) {
							itCount = 0;
						}
						redraw = true;
						break;
					default:
						redraw = true;
						break;
					}
				}
			default:
				break;
			}
		}
	}
	delete task;
	delete a;
	delete[] arr;
} */

int main(int argc, char *argv[])
{
	// SFML Window bauen
	sf::ContextSettings settings;
	settings.antialiasingLevel = 2;
	settings.majorVersion = 0;
	settings.minorVersion = 1;
	
	sf::VideoMode Desktop;
	sf::RenderWindow* window = NULL;
	window = new sf::RenderWindow(Desktop.getDesktopMode(), "Mandelbrot ist lecker", sf::Style::Fullscreen, settings);
	sf::Vector2u size = window->getSize();
	arr = new sf::Uint8[size.x * size.y * 4];
	XW = size.x;
	YW = size.y;
	int ThrCnt = 8;
	while (YW % ThrCnt) {
		ThrCnt++;
		if (ThrCnt > 40) {
			std::cerr << "No valid threadcount found... What resolution are you using?" << std::endl;
			return 1;
		}
	}
	THREADCOUNT = ThrCnt;
	debugstring.append("HW Concurrency: " + std::to_string(std::thread::hardware_concurrency()) + '\n');
	debugstring.append("Threads used:   " + std::to_string(THREADCOUNT) + '\n');
	window->setFramerateLimit(30);
	
	// Color management
	std::array<std::string, 5> colormaps = { "./res/jet.csv", "./res/pink.csv", "./res/parula.csv", "./res/summer.csv", "./res/gray.csv" };
	std::size_t colorCounter = 0;
	if (!loadColors(colormaps.at(colorCounter))) {
		std::cerr << "Could not load colors.";
		return 1;
	}
	// Teig für Mandelbrot anrühren
	CooSys = new CarthCoords(3.0, sf::Vector2<double>(0, 0), sf::Vector2<double>(1.5, 1.5), sf::Vector2i(XW, YW));
	sf::Font font;
	if (!font.loadFromFile("./res/FiraMono-Regular.ttf"))
	{
		std::cerr << "Load font problem" << std::endl;
	}

	std::array<int, 8> iterations = { 100, 500, 1000, 2000, 3000, 5000, 7500, 10000 };
	int itCount = 0;

	text.setFont(font);
	text.setCharacterSize(15);
	text.setFillColor(sf::Color::White);
	sf::Color textColor(20, 20, 20, 255);
	text.setOutlineColor(textColor);
	text.setOutlineThickness(3);
	text.setStyle(sf::Text::Regular);
	if (!texture.create(XW, YW))
	{
		std::cerr << "Could not create texture" << std::endl;
		window->close();
	}
	std::packaged_task<void(double x_cent, double y_cent, double zoom, double max_betrag, int xpixels, int ypixels, int max_iter)>* task = NULL;
	std::future<void> future;
	std::thread * a = NULL;
	std::chrono::milliseconds t1(0);

	// the main loop
	while (window->isOpen()) 
	{
		// Worker für Mandelbrot starten
		if (redraw) {
			task = new std::packaged_task<void(double, double, double, double, int, int, int)>(mandelMain);
			future = task->get_future();
			a = new std::thread(std::move(*task), CooSys.load()->getCenter().x, CooSys.load()->getCenter().y, CooSys.load()->getZoom(), 20, XW, YW, maxIterations.load());
			redraw = false;
			redraw_active = true;
		}
		// worker für Mandelbrot abfragen
		if (redraw_active) {
			std::future_status status = future.wait_for(t1);
			if (status == std::future_status::ready && a != NULL) {
				a->join();
				texture.update(arr);
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
		if (debug || showpos) {
			sf::Vector2i position = sf::Mouse::getPosition(*window);
			sf::Vector2<double> capos = CooSys.load()->pixToCarth(position);
			std::string dadadubs;
			if (showpos) {
				dadadubs = to_string_prec(capos.x) + '|' + to_string_prec(capos.y) + '\n';
			}
			if (debugstring.length() > 500) {
				int firstNewline = debugstring.find_first_of('\n', 1);
				debugstring.erase(0, firstNewline + 1);
			}
			if (debug) {
				dadadubs.append(debugstring);
			}
			text.setString(dadadubs);
			window->draw(text);
		}


		// end the current frame
		window->display();
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window->pollEvent(event))
		{
			// std::cout << "Event has been detected!" << std::endl;
			switch (event.type)
			{
			// Window control Events:
			case sf::Event::Closed:
				window->close();
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
							sf::Vector2i position = sf::Mouse::getPosition(*window);
							CooSys.load()->zoomIn(position.x, position.y, 2.0);
							debugstring.append("Redraw at: X = " + to_string_prec(CooSys.load()->getCenter().x) + " Y = " + to_string_prec(CooSys.load()->getCenter().y) + " with Zoom: " + to_string_prec(CooSys.load()->getZoom()) + "\n");
							redraw = true;
						}
						else {
							// Zoom out
							sf::Vector2i position = sf::Mouse::getPosition(*window);
							CooSys.load()->zoomIn(position.x, position.y, 0.75);
							debugstring.append("Redraw at: X = " + to_string_prec(CooSys.load()->getCenter().x) + " Y = " + to_string_prec(CooSys.load()->getCenter().y) + " with Zoom: " + to_string_prec(CooSys.load()->getZoom()) + "\n");
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
						window->close();
						break;
					case sf::Keyboard::D:
						debug = (debug) ? false : true;
						break;
					case sf::Keyboard::P:
						showpos = (showpos) ? false : true;
						break;
					case sf::Keyboard::C:
						colorCounter++;
						if (colorCounter >= colormaps.size())
							colorCounter = 0;
						debugstring.append("Loading colormap: " + colormaps.at(colorCounter) + '\n');
						loadColors(colormaps.at(colorCounter));
						redraw = true;
						break;
					case sf::Keyboard::I:
						maxIterations = iterations.at(itCount);
						debugstring.append("Changing max iterations to " + std::to_string(iterations.at(itCount)) + '\n');
						itCount++;
						if (itCount >= iterations.size()) {
							itCount = 0;
						}
						redraw = true;
						break;
					default:
						redraw = true;
						break;
					}
				}
			default:
				break;
			}
		}
	}
	// Cleaning:
	delete window;
	delete task;
	delete a;
	delete[] arr;
	return 0;
}
