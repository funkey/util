#include <config.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#ifdef SYSTEM_UNIX
#include <sys/ioctl.h> // to get environment variables
#endif

#include <boost/program_options.hpp>
#include <boost/tuple/tuple.hpp>

#include <util/exceptions.h>
#include <util/foreach.h>
#include <config.h>
#include "ProgramOptions.h"

namespace util {

std::set<program_option_impl*, is_less>*                ProgramOptions::Options = 0;
std::set<std::string>*                                  ProgramOptions::KnownLongParams = 0;
std::map<const program_option_impl*, std::string>       ProgramOptions::Values;
boost::program_options::options_description*            ProgramOptions::CommandLineOptions = 0;
boost::program_options::positional_options_description* ProgramOptions::Positional = 0;
boost::program_options::options_description*            ProgramOptions::ConfigFileOptions  = 0;

ProgramOptionsCleanup programOptionsCleanup;

std::string ProgramOptions::BinaryName = "";

ProgramOption help(
		_long_name = "help",
		_short_name = "h",
		_description_text = "Prints this help.");

ProgramOption optionConfigFile(
		_long_name = "config",
		_short_name = "c",
		_description_text = "Path to a configuration file to use instead of the default [name_of_binary].conf.",
		_argument_sketch = "file");

bool
is_less::operator()(program_option_impl* left, program_option_impl* right) {

	if (left->getModuleName() == right->getModuleName())
		return left->getLongParam() < right->getLongParam();
	else
		return left->getModuleName() < right->getModuleName();
}

void
ProgramOptions::init(int argc, char** argv, bool ignoreUnknown) {

	init(argc, argv, "", ignoreUnknown);
}

void
ProgramOptions::init(std::string configFileName) {

	init(0, NULL, configFileName);
}

void
ProgramOptions::init(int argc, char** argv, std::string configFileName, bool ignoreUnknown) {

	try {

		if (argc > 0)
			BinaryName = argv[0];

		if (Options == 0)
			Options = new std::set<program_option_impl*, is_less>();

		if (KnownLongParams == 0)
			KnownLongParams = new std::set<std::string>();

		// get all options
		boost::program_options::variables_map values;
		if (ignoreUnknown)
			boost::program_options::store(
					boost::program_options::command_line_parser(argc, argv).
					options(*CommandLineOptions).
					positional(*Positional).
					allow_unregistered().
					run(),
					values);
		else
			boost::program_options::store(
					boost::program_options::command_line_parser(argc, argv).
					options(*CommandLineOptions).
					positional(*Positional).
					run(),
					values);
		boost::program_options::notify(values);

		// set initial program option values
		foreach(program_option_impl* option, *Options) {

			if (values.count(option->getLongParam()))
				Values[option] = values[option->getLongParam()].as<std::string>();

			if (option->getModuleName() != "")
				if (values.count(option->getModuleName() + "." + option->getLongParam()))
					Values[option] = values[option->getModuleName() + "." + option->getLongParam()].as<std::string>();
		}

		if (help) {

			printUsage();
			exit(0);
		}

		// add the include option
		ConfigFileOptions->add_options()
				("include",
				boost::program_options::value<std::vector<std::string> >()->composing(),
				"Include another config file.");

		// if config file name was passed, parse it
		if (configFileName.size() > 0) {

			boost::filesystem::path configFile(configFileName);

			if (boost::filesystem::exists(configFile))
				readFromFile(configFile, values);
			else
				std::cerr << "ERROR: configuration file " << configFile << " does not exist" << std::endl;
		}

		// if config file is set, read from it as well
		if (optionConfigFile) {

			boost::filesystem::path configFile(optionConfigFile.as<std::string>());

			if (boost::filesystem::exists(configFile))
				readFromFile(configFile, values);
			else
				std::cerr << "ERROR: configuration file " << configFile << " does not exist" << std::endl;

		// otherwise, try to read from [binary_name].conf
		} else {

			boost::filesystem::path defaultConfigFile(BinaryName + ".conf");

			if (boost::filesystem::exists(defaultConfigFile)) {

				readFromFile(defaultConfigFile, values);
			}
	#ifdef SYSTEM_UNIX
			else {

				// if this does not exist, look in the home directory
				char const* home = getenv("HOME");
				boost::filesystem::path homeConfigFile(std::string(home) + "/.config/" + BinaryName + "/config");

				if (boost::filesystem::exists(homeConfigFile))
					readFromFile(homeConfigFile, values);
			}
	#endif
		}

	} catch (boost::program_options::unknown_option& e) {

		UTIL_THROW_EXCEPTION(
				UsageError,
				"unknown option: " << e.what());
	}
}

void
ProgramOptions::readFromFile(boost::filesystem::path configFile, boost::program_options::variables_map& values) {

	std::size_t beginNewInclude;

	if (values.count("include"))
		beginNewInclude = values["include"].as<std::vector<std::string> >().size();
	else
		beginNewInclude = 0;

	std::ifstream config(configFile.string().c_str());

	if (!config.good()) {

		UTIL_THROW_EXCEPTION(
			IOError,
			"can't open config file: " << configFile << std::endl);
	}

	// read from the config file
	boost::program_options::store(boost::program_options::parse_config_file(config, *ConfigFileOptions), values);
	boost::program_options::notify(values);

	// update program option values
	foreach(program_option_impl* option, *Options) {

		std::string module = (option->getModuleName() != "" ? option->getModuleName() + "." : "");

		// set without module part? (shortcut, only for command line arguments)
		if (values.count(option->getLongParam())) {

			Values[option] = values[option->getLongParam()].as<std::string>();

		// set with module part?
		} else if (values.count(module + option->getLongParam())) {

			Values[option] = values[module + option->getLongParam()].as<std::string>();
		}
	}

	std::size_t endNewInclude = beginNewInclude;

	if(values.count("include"))
		endNewInclude = values["include"].as<std::vector<std::string> >().size();

	// are there new includes?
	if (endNewInclude > beginNewInclude) {

		// include other config file back to front, such that the last one 
		// specified has precedence
		for (int i = (int)endNewInclude - 1; i >= (int)beginNewInclude; i--) {

			std::string includeFileName = values["include"].as<std::vector<std::string> >()[i];

			// append include file name to current config file's directory
			boost::filesystem::path includeFile = configFile.parent_path() / includeFileName;

			if (!boost::filesystem::exists(includeFile)) {

				UTIL_THROW_EXCEPTION(
					IOError,
					"include file " << includeFile << " does not exist");
			}

			readFromFile(includeFile, values);
		}
	}
}

bool
ProgramOptions::isOptionSet(const program_option_impl& option) {

	return Values.find(&option) != Values.end();
}

std::string
ProgramOptions::getOptionValue(const program_option_impl& option) {

	if (Values.count(&option))
		return Values[&option];
	else
		return option.getDefaultValue();
}

void
ProgramOptions::setOptionValue(std::string longName, std::string value) {

	foreach(const program_option_impl* option, *Options) {
		if (option->getLongParam().compare(longName) == 0)
			Values[option] = value;
	}
}

void
ProgramOptions::addProgramOption(program_option_impl* option) {

	if (Options == 0)
		Options = new std::set<program_option_impl*, is_less>();

	if (KnownLongParams == 0)
		KnownLongParams = new std::set<std::string>();

	if (CommandLineOptions == 0)
		CommandLineOptions = new boost::program_options::options_description("command line options");

	if (Positional == 0)
		Positional = new boost::program_options::positional_options_description();

	if (ConfigFileOptions == 0)
		ConfigFileOptions = new boost::program_options::options_description("config file options");

	std::string moduleName = (option->getModuleName() != "" ? option->getModuleName() + "." : "");

	std::string argument = option->getLongParam() + (option->getShortParam() != "" ? "," + option->getShortParam() : "");

	// add the option to the boost options descriptions

	// to the command line options (--Module.longParam and -shortParam)
	CommandLineOptions->add_options()
			((moduleName + argument).c_str(),
			boost::program_options::value<std::string>()->implicit_value("true"),
			"");

	// to the config file options ([Module] longParam)
	ConfigFileOptions->add_options()
			((moduleName + option->getLongParam()).c_str(),
			boost::program_options::value<std::string>()->implicit_value("true"),
			"");

	// try adding the option without the module name (--longParam)
	// (this might be ambiguous)
	if (moduleName != "" &&
	    KnownLongParams->count(option->getLongParam()) == 0)
		CommandLineOptions->add_options()
				(option->getLongParam().c_str(),
				boost::program_options::value<std::string>()->implicit_value("true"),
				"");

	// set it as the positional program option, if desired
	if (option->isPositional())
		Positional->add((moduleName + option->getLongParam()).c_str(), -1);

	Options->insert(option);
	KnownLongParams->insert(option->getLongParam());
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

void
printBlock(std::string text, unsigned int indentation, unsigned int lineWidth, std::ostream& os) {

	while (text.length() > 0) {

		std::string line = extractLine(text, lineWidth);

		size_t pos_new = line.find_first_of("\n");
		// handle lines containing newlines separatly
		if (pos_new != std::string::npos) {
			std::string shortline = line.substr(0, pos_new + 1);
			os << shortline;
			// add remainer to text again
			// and restart loop
			text = line.substr(pos_new + 1, line.length()) + " " + text;
			os << std::string(indentation, ' ');
			continue;
		}

		// stretch every line except last one
		if (text.length() > 0 || pos_new != std::string::npos)
			os << stretchLine(line, lineWidth) << std::endl;
		else
			os << line << std::endl;

		os << std::string(indentation, ' ');
	}
}

void
ProgramOptions::printUsage() {

	std::string moduleName = "";

	std::cout << std::endl
						<< "Usage: " << BinaryName
						<< " --[\e[4moption\e[0m [\e[4mvalue\e[0m]] ..."
						<< std::endl << std::endl;

	std::string optionsText;

	optionsText +=
			"Options can be given on the command line or via a configuration file "
			"(default: " + BinaryName + ".conf):\n" +
			"\n" +
			"  " + BinaryName + " --Module.option value\n" +
			"\n" +
			"is equivalent to giving a configuration file containing\n" +
			"\n" +
			"  [Module]\n" +
			"  option=value\n" +
			"\n" +
			"Command line arguments overwrite config file settings. The module part " +
			"of an option may be omitted on the command line. In this case, all options " +
			"with that name in all modules are set. Short versions of options are " +
			"available for the command line as shown below.\n" +
			"\n";

	// get tty width:
	#ifdef SYSTEM_WINDOWS
		int numColumns = 80;
	#else
		winsize wsize;
		ioctl(0, TIOCGWINSZ, &wsize);
		int numColumns = std::min(wsize.ws_col - 5, 100);
	#endif

	printBlock(optionsText, 0, numColumns, std::cout);

	std::cout << "General options:" << std::endl << std::endl;

	std::set<program_option_impl*>::iterator i;
	for (i = Options->begin(); i != Options->end(); i++) {

		if (moduleName != (*i)->getModuleName()) {

			moduleName = (*i)->getModuleName();
			std::cout << std::endl << "Module '" << moduleName << "':" << std::endl << std::endl;
		}

		std::cout << *(*i) << std::endl;
	}
}

std::string
program_option_impl::getModuleName() const {
	return _moduleName;
}

std::string
program_option_impl::getLongParam() const {
	return _longParam;
}

std::string
program_option_impl::getShortParam() const {
	return _shortParam;
}

std::string
program_option_impl::getDescription() const {
	return _description;
}

std::string
program_option_impl::getArgumentSketch() const {
	return _argumentSketch;
}

std::string
program_option_impl::getDefaultValue() const {
	return _defaultValue;
}

bool
program_option_impl::isPositional() const {
	return _isPositional;
}

std::ostream&
operator<<(std::ostream& os, program_option_impl& option) {

	int sizeLongParam       = option.getLongParam().length();
	int sizeShortParam      = (option.getShortParam() != "" ? option.getShortParam().length() + 1 : 0);
	int sizeArgumentSketch  = option.getArgumentSketch().length();

	// get tty width:
#ifdef SYSTEM_WINDOWS
	int numColumns = 80;
#else
	winsize wsize;
	ioctl(0, TIOCGWINSZ, &wsize);
	int numColumns = std::min(wsize.ws_col - 5, 100);
#endif

	int spaceIndentation = 2;
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
	
	os << std::string(std::max(0, spaceShortParam - sizeShortParam), ' ');

	os << option.getLongParam() << " \e[4m" << option.getArgumentSketch() << "\e[0m"; 

	os << std::string(std::max(0, spaceLongParam - (sizeLongParam + 1 + sizeArgumentSketch)), ' ');

	std::string description = option.getDescription();

	printBlock(description, spaceIndentation + spaceLongParam + spaceShortParam, spaceDescription, os);

	return os;
}

} // namespace util

// End of file
