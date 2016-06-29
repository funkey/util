#ifndef UTIL_STRING_H__
#define UTIL_STRING_H__

#include <string>
#include <sstream>
#include <vector>

std::vector<std::string> split(const std::string& s, char delim) {

	std::vector<std::string> tokens;
	std::stringstream ss(s);
	std::string token;

	while (getline(ss, token, delim))
		tokens.push_back(token);

	return tokens;
}

#endif // UTIL_STRING_H__

