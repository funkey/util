#include <iostream>

#include <util/ProgramOptions.h>

using namespace util;

util::ProgramOption frotzOption(
		_module = "FrotzBackend",
		_long_name = "frotz",
		_short_name = "f",
		_description_text = "The frotz value",
		_default_value = "-1");

util::ProgramOption flitzOption(
		_module = "FlitzBackend",
		_long_name = "flitz",
		_description_text = "The important flitz value. Set it with causion!",
		_default_value = 0.1);

int main(int argc, char** argv) {

	util::ProgramOptions::init(argc, argv);

	if (frotzOption) {

		int f = frotzOption;

		std::cout << "frotz set to " << (int)frotzOption << std::endl;

	} else {

		std::cout << "frotz option is not set" << std::endl;
	}

	std::cout << "The important flitz value is " << (float)flitzOption << std::endl;
}
