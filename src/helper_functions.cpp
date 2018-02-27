#include "../hdr/helper_functions.hpp"

color ColorMap[C_MAP_LEN];

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

std::string to_string_prec(const double a_value, int prec) {
	std::ostringstream out;
	out << std::setprecision(prec) << a_value;
	return out.str();
}