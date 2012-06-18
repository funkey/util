#ifndef COMMANDLINEPARSER_H_
#define COMMANDLINEPARSER_H_

#include <iostream>
#include <string>
#include <set>
#include <map>

#include <boost/lexical_cast.hpp>
#include <boost/parameter.hpp>
#include <boost/program_options.hpp>

#include <util/typename.h>

namespace util {

// forward declarations
template <typename TargetType>
class OptionConverter;
class program_option_impl;

// for sorting in the option set
struct is_less {

	bool operator()(program_option_impl* left, program_option_impl* right);
};

BOOST_PARAMETER_NAME(module)
BOOST_PARAMETER_NAME(long_name)
BOOST_PARAMETER_NAME(short_name)
BOOST_PARAMETER_NAME(description_text)
BOOST_PARAMETER_NAME(argument_sketch)
BOOST_PARAMETER_NAME(default_value)

class ProgramOptions {

public:

	static void init(int argc, char** argv);

	static bool isOptionSet(program_option_impl& option);

	static std::string getOptionValue(program_option_impl& option);

	static void addProgramOption(program_option_impl* option);

	static void printUsage();

private:
	
	ProgramOptions() {};

	static std::set<program_option_impl*, is_less>* Options;

	static std::map<program_option_impl*, std::string> Values;

	static boost::program_options::options_description* BoostOptions;

	static std::string	BinaryName;
};

class program_option_impl {

public:

	template <class ArgumentPack>
	program_option_impl(const ArgumentPack& args) :

		_moduleName(args[_module | ""]),
		_longParam(args[_long_name]),
		_shortParam(args[_short_name | ""]),
		_description(args[_description_text | ""]),
		_argumentSketch(args[_argument_sketch | ""]),
		_defaultValue(boost::lexical_cast<std::string>(args[_default_value | ""])) {

		ProgramOptions::addProgramOption(this); 
	}

	std::string getModuleName();

	std::string getLongParam();

	std::string getShortParam();

	std::string getDescription();

	std::string getArgumentSketch();

	std::string getDefaultValue();

	/**
	 * Defines a lexicographic order on command line
	 * options.
	 */
	bool operator<(program_option_impl* right);

	/**
	 * Implicit conversion to ValueType. This allows to convert the value of
	 * the program option into whatever type you expect. For instance, you
	 * can write for a ProgramOption fooOption
	 *
	 *   int foo = fooOption;
	 *
	 * as long as the argument given to fooOption can be converted to int.
	 *
	 * This conversion also allows to check if an option is set via
	 *
	 *   if (fooOption)
	 *     ...
	 */
	template <typename ValueType>
	operator ValueType() {

		OptionConverter<ValueType> converter;

		return converter(*this);
	}

private:

	std::string _moduleName;

	std::string _longParam;

	std::string _shortParam;

	std::string _description;

	std::string _argumentSketch;

	std::string _defaultValue;
};

class ProgramOption : public program_option_impl {

	public:

		BOOST_PARAMETER_CONSTRUCTOR(
			ProgramOption,
			(program_option_impl),
			tag,
			(required
				(long_name, *))
			(optional
				(short_name, *)
				(module, *)
				(description_text, *)
				(argument_sketch, *)))
};

template <typename TargetType>
struct OptionConverter {

	TargetType operator()(program_option_impl& option) {

		return boost::lexical_cast<TargetType>(ProgramOptions::getOptionValue(option));
	}
};

template <>
struct OptionConverter<bool> {

	bool operator()(program_option_impl& option) {

		return ProgramOptions::isOptionSet(option);
	}
};

std::ostream&
operator<<(std::ostream& os, program_option_impl& option);

} // namespace util

#endif /*COMMANDLINEPARSER_H_*/
