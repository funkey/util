#include "string.h"

std::vector<std::string> split(const std::string& s, char delim) {

	std::vector<std::string> tokens;
	std::stringstream ss(s);
	std::string token;

	while (getline(ss, token, delim))
		tokens.push_back(token);

	return tokens;
}
