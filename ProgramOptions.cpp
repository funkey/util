#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sys/ioctl.h> // to get environment variables

#include <boost/program_options.hpp>
#include <boost/tuple/tuple.hpp>

#include <util/foreach.h>
#include "ProgramOptions.h"

namespace util {

std::set<program_option_impl*, is_less>*     ProgramOptions::Options = 0;
std::map<program_option_impl*, std::string>  ProgramOptions::Values;
boost::program_options::options_description* ProgramOptions::BoostOptions = 0;

std::string ProgramOptions::BinaryName = "";

ProgramOption help(_long_name = "help", _short_name = "h", _description_text = "Prints this help.");
ProgramOption configFile(_long_name = "config", _short_name = "c", _description_text = "Path to a configuration file.");

bool
is_less::operator()(program_option_impl* left, program_option_impl* right) {

	if (left->getModuleName() == right->getModuleName())
		return left->getLongParam() < right->getLongParam();
	else
		return left->getModuleName() < right->getModuleName();
}

void
ProgramOptions::init(int argc, char** argv) {

	BinaryName = argv[0];

	if (Options == 0)
		Options = new std::set<program_option_impl*, is_less>();

	// get all options
	boost::program_options::variables_map values;
	std::string                                  name;
	boost::program_options::options_description* module;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, *BoostOptions), values);
	boost::program_options::notify(values);

	// set initial program option values
	foreach(program_option_impl* option, *Options) {

		if (values.count(option->getLongParam()))
			Values[option] = values[option->getLongParam()].as<std::string>();
	}

	if (help) {

		printUsage();
		exit(0);
	}

	// if config file is set, read from it as well
	if (configFile) {

		std::string configFileName = configFile;

		std::cout << "reading configuration from file " << configFileName << std::endl;

		std::ifstream config(configFileName.c_str());

		boost::program_options::store(boost::program_options::parse_config_file(config, *BoostOptions), values);

	// otherwise, try to read from [binary_name].conf
	} else {

		std::cout << "trying to read configuration from file " << BinaryName << ".conf" << std::endl;

		std::ifstream config((BinaryName + ".conf").c_str());

		boost::program_options::store(boost::program_options::parse_config_file(config, *BoostOptions), values);
	}

	boost::program_options::notify(values);

	// update program option values
	foreach(program_option_impl* option, *Options) {

		if (values.count(option->getLongParam()))
			Values[option] = values[option->getLongParam()].as<std::string>();
	}
}

bool
ProgramOptions::isOptionSet(program_option_impl& option) {

	return Values.find(&option) != Values.end();
}

std::string
ProgramOptions::getOptionValue(program_option_impl& option) {

	if (Values.count(&option))
		return Values[&option];
	else
		return option.getDefaultValue();
}

void
ProgramOptions::addProgramOption(program_option_impl* option) {

	if (Options == 0)
		Options = new std::set<program_option_impl*, is_less>();

	if (BoostOptions == 0)
		BoostOptions = new boost::program_options::options_description("boost options");

	ProgramOptions::Options->insert(option);

	std::string moduleName = option->getModuleName();

	std::string argument = option->getLongParam() + (option->getShortParam() != "" ? "," + option->getShortParam() : "");

	// add the option to the boost options description
	BoostOptions->add_options()(argument.c_str(), boost::program_options::value<std::string>()->implicit_value(""), "");
}

void
ProgramOptions::printUsage() {

	std::string moduleName = "";

	std::cout << std::endl
						<< "Usage: " << BinaryName
						<< " [\e[4moption\e[0m [\e[4mvalue\e[0m]] ..."
						<< std::endl << std::endl;

	std::cout << "General options:" << std::endl << std::endl;

	std::set<program_option_impl*>::iterator i;
	for (i = Options->begin(); i != Options->end(); i++) {

		if (moduleName != (*i)->getModuleName()) {

			moduleName = (*i)->getModuleName();
			std::cout << std::endl << moduleName << " options:" << std::endl << std::endl;
		}

		std::cout << *(*i) << std::endl;
	}
}

std::string
program_option_impl::getModuleName() {
	return _moduleName;
}

std::string
program_option_impl::getLongParam() {
	return _longParam;
}

std::string
program_option_impl::getShortParam() {
	return _shortParam;
}

std::string
program_option_impl::getDescription() {
	return _description;
}

std::string
program_option_impl::getArgumentSketch() {
	return _argumentSketch;
}

std::string
program_option_impl::getDefaultValue() {
	return _defaultValue;
}

std::string
extractLine(
		std::string& str, size_t lineWidth,
		std::string breakBefore = " \t",
		std::string breakAfter	= "-") {

	// set break position behind last character initially
	size_t breakPosition = str.length();

	bool doBreakAfter = false;

	// try to find a break position that fits a line
	while ((doBreakAfter ? breakPosition - 1 : breakPosition) > lineWidth) {

		int nextPossibleBreakBeforePosition =
			str.find_last_of(breakBefore, breakPosition - 1);

		int nextPossibleBreakAfterPosition =
			str.find_last_of(breakAfter, breakPosition - 1);

		size_t nextPossibleBreakPosition;
		bool doPossibleBreakAfter;
		if (nextPossibleBreakAfterPosition > nextPossibleBreakBeforePosition) {

			doPossibleBreakAfter = true;
			nextPossibleBreakPosition = nextPossibleBreakAfterPosition;

		} else {

			doPossibleBreakAfter = false;
			nextPossibleBreakPosition = nextPossibleBreakBeforePosition;
		}

		if (nextPossibleBreakPosition == std::string::npos) {

			break;

		} else {

			breakPosition = nextPossibleBreakPosition;
			doBreakAfter = doPossibleBreakAfter;
		}
	}

	if (doBreakAfter)
		breakPosition++;

	std::string line = str.substr(0, breakPosition);

	// remove line from str and delete trailing spaces from remainer
	size_t startSubString = str.find_first_not_of(' ', breakPosition);
	if (startSubString == std::string::npos)
		startSubString = breakPosition;
	str = str.substr(startSubString, str.length());
	
	return line;
}

std::string
stretchLine(std::string line, unsigned width) {

	int difference = width - line.length();

	if (difference <= 0)
		return line;

	// split line by spaces:
	std::vector<std::string> words;

	while (true) {

		size_t spacePos = line.find_first_of(' ');
		if (spacePos == std::string::npos) {
			// no more spaces found
			words.push_back(line);
			break;
		}
		words.push_back(line.substr(0, spacePos));
		line = line.substr(spacePos + 1);
	}

	int numSpaces = words.size() - 1;

	// number of spaces need to be added to every space in the line
	int equallyAugmentSpaces		= (numSpaces != 0 ? difference / numSpaces : 0);
	// number of spaces that have to be augmented by a single space additionally
	int additionalAugmentSpaces = (numSpaces != 0 ? difference % numSpaces : 0);

	std::string stretchedLine = "";

	std::vector<std::string>::iterator i;
	for (i = words.begin(); i != words.end(); i++) {

		if (i != words.begin()) {
			stretchedLine += std::string(equallyAugmentSpaces + 1, ' ');
			if (additionalAugmentSpaces > 0) {
				stretchedLine += ' ';
				additionalAugmentSpaces--;
			}
		}

		stretchedLine += (*i);
	}
	
	return stretchedLine;
}
 
std::ostream&
operator<<(std::ostream& os, program_option_impl& option) {

	int sizeLongParam       = option.getLongParam().length() + 2;
	int sizeShortParam      = (option.getShortParam() != "" ? option.getShortParam().length() + 1: 0);
	int sizeArgumentSketch  = option.getArgumentSketch().length();

	// get tty width:
	winsize wsize;
	ioctl(0, TIOCGWINSZ, &wsize);
	int numColumns = std::min(wsize.ws_col - 5, 100);

	int spaceIndentation = 4;
	int spaceLongParam   = 25;
	int spaceShortParam  = 5;
	int spaceDescription =
			numColumns
			- spaceIndentation
			- spaceLongParam
			- spaceShortParam;

	os << std::string(spaceIndentation, ' ');

	if (option.getShortParam() != "")
		os << "-" << option.getShortParam();
	
	os << std::string(spaceShortParam - sizeShortParam, ' ');

	os << "--" << option.getLongParam() << " \e[4m" << option.getArgumentSketch() << "\e[0m"; 

	os << std::string(spaceLongParam - (sizeLongParam + 1 + sizeArgumentSketch), ' ');

	std::string description = option.getDescription();

	while (description.length() > 0) {

		std::string line = extractLine(description, spaceDescription);

		size_t pos_new = line.find_first_of("\n");
		// handle lines containing newlines separatly
		if (pos_new != std::string::npos) {
			std::string shortline = line.substr(0, pos_new + 1);
			os << shortline;
			// add remainer to description again
			// and restart loop
			description = line.substr(pos_new + 1, line.length()) + " " + description;
			os << std::string(spaceIndentation + spaceLongParam + spaceShortParam , ' ');
			continue;
		}

		// stretch every line except last one
		if (description.length() > 0 || pos_new != std::string::npos)
			os << stretchLine(line, spaceDescription) << std::endl;
		else
			os << line << std::endl;
			
		os << std::string(spaceIndentation + spaceLongParam + spaceShortParam , ' ');
	}

	return os;
}

} // namespace util

// End of file
