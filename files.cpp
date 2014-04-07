#include "files.h"

std::string readline(std::ifstream& s) {

	char b[UTIL_READLINE_BUFFER_SIZE];

	std::string line;

	while (true) {

		s.getline(b, UTIL_READLINE_BUFFER_SIZE);

		line += b;

		if (s.eof())
			break;

		if (s.fail()) {

			s.clear();
			continue;
		}

		if (s.gcount() <= UTIL_READLINE_BUFFER_SIZE)
			break;
	}

	return line;
}
