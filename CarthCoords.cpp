#include "CarthCoords.h"

CarthCoords::CarthCoords(double zoom, double centerX, double centerY, double offsetX, double offsetY, int xPixMax, int yPixMax)
{
	this->setVariables(zoom, sf::Vector2<double>(centerX, centerY), sf::Vector2<double>(offsetX, offsetY), sf::Vector2i(xPixMax, yPixMax));
}

CarthCoords::CarthCoords(double zoom, sf::Vector2<double> center, sf::Vector2<double> offset, sf::Vector2i PixMax)
{
	this->setVariables(zoom, center, offset, PixMax);
}

CarthCoords::~CarthCoords()
{

}

/// <returns>Returns a SFML 2D double vector of the coordinate system offset.</returns>
sf::Vector2<double> CarthCoords::getOffset()
{
	return this->offset;
}

/// <returns>Returns a SFML 2D double vector of the window center in carthesian coordinates.</returns>
sf::Vector2<double> CarthCoords::getCenter()
{
	return this->center;
}

/// <returns>Returns a double of the coordinate system zoom.</returns>
double CarthCoords::getZoom()
{
	return this->zoom;
}

void CarthCoords::zoomIn(int x_pix, int y_pix, double fact)
{
	sf::Vector2<double> transformed = this->pixToCarth(sf::Vector2i(x_pix, y_pix));
	this->zoomIn(transformed.x, transformed.y, fact);
}

void CarthCoords::zoomIn(double x, double y, double fact)
{
	double newZoom = this->zoom / fact;
	
	this->offset.x = (this->offset.x - (x - this->center.x)) - (this->zoom - newZoom) / 2;
	this->offset.y = (this->offset.y + (y - this->center.y)) - ((this->zoom - newZoom) / 2 * ((this->y_pixMax * 1.0) / this->x_pixMax));

	this->zoom = newZoom;
	this->x_max = x + (this->zoom / 2);
	this->x_min = x - (this->zoom / 2);
	this->y_max = y + ((this->zoom / 2) * ((this->y_pixMax * 1.0) / this->x_pixMax));
	this->y_min = y - ((this->zoom / 2) * ((this->y_pixMax * 1.0) / this->x_pixMax));

	this->center.x = x;
	this->center.y = y;
}

/// <returns>Returns a SFML 2D double vector of the coordinate system equivalent to the pixel input.</returns>
sf::Vector2<double> CarthCoords::pixToCarth(sf::Vector2i pos)
{
	sf::Vector2<double> retval;

	retval.x = ((pos.x * 1.0) / this->x_pixMax) * (this->x_max - this->x_min) - this->offset.x;

	retval.y = this->offset.y - (((pos.y * 1.0) / this->y_pixMax) * (this->y_max - this->y_min));

	return retval;
}

/// <returns>Returns a SFML 2D integer vector of the pixel system equivalent to the carthesian input.</returns>
sf::Vector2i CarthCoords::carthToPix(sf::Vector2<double> pos)
{
	sf::Vector2i retval;

	retval.x = (int)std::floor(this->x_pixMax * (pos.x + this->offset.x) / (this->x_max - this->x_min));

	retval.y = (int)std::floor(this->y_pixMax * (this->offset.y - pos.y) / (this->y_max - this->y_min));

	return retval;
}

void CarthCoords::setVariables(double zoom, sf::Vector2<double> center, sf::Vector2<double> offset, sf::Vector2i PixMax)
{
	this->zoom = zoom;	
	this->center = center;
	this->x_pixMax = PixMax.x;
	this->y_pixMax = PixMax.y;
	this->x_max = center.x + (this->zoom / 2);
	this->x_min = center.x - (this->zoom / 2);
	this->y_max = center.y + ((this->zoom / 2) * ((this->y_pixMax * 1.0) / this->x_pixMax));
	this->y_min = center.y - ((this->zoom / 2) * ((this->y_pixMax * 1.0) / this->x_pixMax));
	this->offset.x = offset.x;
	this->offset.y = offset.y * ((this->y_pixMax * 1.0) / this->x_pixMax);
}
