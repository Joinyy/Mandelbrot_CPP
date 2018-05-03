#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "Proj.hpp"

extern color ColorMap[C_MAP_LEN];

bool loadColors(std::string);
std::string to_string_prec(double, int prec = 15);
