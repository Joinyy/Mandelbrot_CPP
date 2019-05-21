#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "Proj.hpp"

class color {
private:
    // Nothing to hide
public:
    unsigned char R;
    unsigned char G;
    unsigned char B;
    unsigned char A;
    color();
	color(const color & in);
    color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    color(rgb in);
	void set(color & in);
	std::string tostring();
	color & operator=(const color & in);
};


extern color ColorMap[C_MAP_LEN];

bool loadColors(std::string);
bool loadColorGradient(color &, color &, unsigned int);
std::string to_string_prec(double, int prec = 15);

static hsv rgb2hsv(rgb in);
static rgb hsv2rgb(hsv in);
double interpol_linear(double a, double b, double t);
color interpolRGB(color & start, color & end, double fraction);