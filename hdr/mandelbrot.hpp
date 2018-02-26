#pragma once

#include <cmath>
#include <thread>
#include <vector>

#include "Proj.hpp"

extern sf::Vertex **arr;

void mandelMain(double x_cent, double y_cent, double zoom, double max_betrag, int xpixels, int ypixels, int max_iter);