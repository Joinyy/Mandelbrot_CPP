#include "../hdr/helper_functions.hpp"

color ColorMap[C_MAP_LEN];

color::color() {
    this->R = 255;
    this->G = 255;
    this->B = 255;
    this->A = 255;
}

color::color(const color & in) {
	this->R = in.R;
	this->G = in.G;
	this->B = in.B;
	this->A = in.A;
}

color::color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) {
    this->R = (r <= 255) ? r : 255;
    this->G = (g <= 255) ? g : 255;
    this->B = (b <= 255) ? b : 255;
    this->A = (a <= 255) ? a : 255;
}

color::color(rgb in) {
    this->R = (unsigned char) floor(in.r * 255);
    this->G = (unsigned char) floor(in.g * 255);
    this->B = (unsigned char) floor(in.b * 255);
    this->A = 255;
}

color & color::operator=(const color & in) {
	this->R = in.R;
	this->G = in.G;
	this->B = in.B;
	this->A = in.A;
	return *this;
}

void color::set(color & in) {
	this->R = in.R;
	this->G = in.G;
	this->B = in.B;
	this->A = in.A;
}

std::string color::tostring() {
	return "R(" + std::to_string((int)this->R) + ") G(" + std::to_string((int)this->G) + ") B(" + std::to_string((int)this->B) + ")";
}

// fraction [0.0 .. 1.0]
color interpolRGB(color & start, color & end, double fraction) {
	rgb retVal = {0, 0, 0};
	hsv start_hsv = rgb2hsv({start.R / 255.0, start.G / 255.0, start.B / 255.0});
	hsv end_hsv   = rgb2hsv({end.R / 255.0, end.G / 255.0, end.B / 255.0});
	hsv interpolResult = {0, 0, 0};
	interpolResult.h = interpol_linear(start_hsv.h, end_hsv.h, fraction);
	interpolResult.s = interpol_linear(start_hsv.s, end_hsv.s, fraction);
	interpolResult.v = interpol_linear(start_hsv.v, end_hsv.v, fraction);

	retVal = hsv2rgb(interpolResult);
	return color(retVal);
}

double interpol_linear(double a, double b, double t) {
	return a * (1.0 - t) + b * t;
}

hsv rgb2hsv(rgb in) {
	hsv out;
	double min, max, delta;

	min = (in.r < in.g) ? in.r : in.g;
	min = (min  < in.b) ? min  : in.b;

	max = (in.r > in.g) ? in.r : in.g;
	max = (max  > in.b) ? max  : in.b;

	out.v = max;
	delta = max - min;
	if (delta < 0.00001) {
		out.s = 0;
		out.h = 0; // undefined, maybe nan?
		return out;
	}
	if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
		out.s = (delta / max); // s
	} else {
		// if max is 0, then r = g = b = 0
		// s = 0, h is undefined
		out.s = 0.0;
		out.h = NAN; // its now undefined
		return out;
	}
	if( in.r >= max ) {// > is bogus, just keeps compilor happy
		out.h = ( in.g - in.b ) / delta; // between yellow & magenta
	} else {
		if( in.g >= max ) {
			out.h = 2.0 + ( in.b - in.r ) / delta; // between cyan & yellow
		} else {
			out.h = 4.0 + ( in.r - in.g ) / delta; // between magenta & cyan
		}
	}
	out.h *= 60.0; // degrees

	if( out.h < 0.0 ) {
		out.h += 360.0;
	}
	return out;
}

rgb hsv2rgb(hsv in)
{
	double hh, p, q, t, ff;
	long i;
	rgb out;

	if(in.s <= 0.0) { // < is bogus, just shuts up warnings
		out.r = in.v;
		out.g = in.v;
		out.b = in.v;
		return out;
	}
	hh = in.h;
	if(hh >= 360.0) {
		hh = 0.0;
	}
	hh /= 60.0;
	i = (long)hh;
	ff = hh - i;
	p = in.v * (1.0 - in.s);
	q = in.v * (1.0 - (in.s * ff));
	t = in.v * (1.0 - (in.s * (1.0 - ff)));

	switch(i) {
	case 0:
		out.r = in.v;
		out.g = t;
		out.b = p;
		break;
	case 1:
		out.r = q;
		out.g = in.v;
		out.b = p;
		break;
	case 2:
		out.r = p;
		out.g = in.v;
		out.b = t;
		break;
	case 3:
		out.r = p;
		out.g = q;
		out.b = in.v;
		break;
	case 4:
		out.r = t;
		out.g = p;
		out.b = in.v;
		break;
	case 5:
	default:
		out.r = in.v;
		out.g = p;
		out.b = q;
		break;
	}
	return out;	 
}