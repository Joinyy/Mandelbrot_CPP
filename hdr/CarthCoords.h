#pragma once

#include <SFML/System/Vector2.hpp>
#include <cmath>

/// Class for managing a carthesian coordinate system in a integer based pixel view
class CarthCoords
{
public:
	// For full functionality after construction these variables are needed:
	// zoom, center, offset, x_pixMax, y_pixMax
	CarthCoords(double zoom, double centerX, double centerY, double offsetX, double offsetY, int xPixMax, int yPixMax);
	CarthCoords(double zoom, sf::Vector2<double> center, sf::Vector2<double> offset, sf::Vector2i PixMax);
	~CarthCoords();

	sf::Vector2<double> getOffset();
	sf::Vector2<double> getCenter();
	double getZoom();

	void zoomIn(int x_pix, int y_pix, double fact = 1.5);
	void zoomIn(double x, double y, double fact = 1.5);

	sf::Vector2<double> pixToCarth(sf::Vector2i pos);
	sf::Vector2i carthToPix(sf::Vector2<double> pos);

private:
	void setVariables(double zoom, sf::Vector2<double> center, sf::Vector2<double> offset, sf::Vector2i PixMax);
	// Zoom is calculated for x-Range
	double zoom;
	// These are needed for computations
	double x_max;
	double x_min;
	double y_max;
	double y_min;

	// Pixelrange for calculation
	int x_pixMax;
	int y_pixMax;

	// Offset to window center
	sf::Vector2<double> offset;
	// Actual window center values in carth
	sf::Vector2<double> center;
};

