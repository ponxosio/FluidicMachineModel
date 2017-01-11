/*
 * Utils.cpp
 *
 *  Created on: 3 de mar. de 2016
 *      Author: angel
 */

#include "Utils.h"

Utils::Utils() {
	// TODO Auto-generated constructor stub

}

Utils::~Utils() {
	// TODO Auto-generated destructor stub
}

string Utils::getCurrentDir() throw (std::runtime_error) {
	string vuelta = "";
    /*char win32[FILENAME_MAX];

	if (GetCurrentDir(win32, sizeof(win32))) {
		vuelta = string(win32);
	} else {
        throw (std::runtime_error("Graph::saveGraph-> unable to retrieve current directory"));
    }*/
	return vuelta;
}

void Utils::tokenize(const string& str, vector<string>& tokens,
		const string& delimiters) {
	// Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos) {
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}

long Utils::getCurrentTimeMilis() {
	using namespace std::chrono;
	milliseconds ms = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch());
	return ms.count();
}

bool Utils::ends_with(std::string const &a, std::string const &b) {
	auto len = b.length();
	auto pos = a.length() - len;
	if (pos < 0)
		return false;
	auto pos_a = &a[pos];
	auto pos_b = &b[0];
	while (*pos_a)
		if (*pos_a++ != *pos_b++)
			return false;
	return true;
}

bool Utils::starts_with(std::string const &a, std::string const &b) {
	bool starts = false;
	auto lenb = b.length();
	auto lena = a.length();

	if (a >= b) {
		starts = true;
		auto pos_a = &a[0];
		auto pos_b = &b[0];

		while (*pos_b && starts)
			starts = (*pos_a++ == *pos_b++);
				
	}
	return starts;
}

size_t Utils::strToSizeT(const std::string & str) throw(std::invalid_argument) {
    size_t value;
    if (sscanf(str.c_str(), "%zu", &value)) {
        return value;
    }  else {
        throw(std::invalid_argument("\"" + str + "\" not a number."));
    }
}

size_t Utils::cantorParingFunction(int k1, int k2) {
    return (0.5 * (k1 + k2) * (k1 + k2 + 1)) + k2;
}

size_t Utils::cantorParingFunction(long k1, long k2) {
    return (0.5 * (k1 + k2) * (k1 + k2 + 1)) + k2;
}

size_t Utils::cantorParingFunction(short int k1, short int k2) {
    return (0.5 * (k1 + k2) * (k1 + k2 + 1)) + k2;
}
