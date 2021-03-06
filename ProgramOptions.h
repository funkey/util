#ifndef COMMANDLINEPARSER_H_
#define COMMANDLINEPARSER_H_

#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/parameter.hpp>
#include <boost/program_options.hpp>

#include <util/typename.h>
#include <util/exceptions.h>

namespace util {

// forward declarations
template <typename TargetType>
struct OptionConverter;
class program_option_impl;
struct ProgramOptionsCleanup;

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
BOOST_PARAMETER_NAME(is_positional)

class ProgramOptions {

public:

	/**
	 * Init ProgramOptions from command line arguments.
	 */
	static void init(int argc, char** argv, bool ignoreUnknown = false);

	/**
	 * Init ProgramOptions from a config file.
	 */
	static void init(std::string configFile);

	static bool isOptionSet(const program_option_impl& option);

	static std::string getOptionValue(const program_option_impl& option);

	static void setOptionValue(std::string longName, std::string value);

	static void addProgramOption(program_option_impl* option);

	static void printUsage();

private:

	friend struct ProgramOptionsCleanup;

	ProgramOptions() {};

	static void init(int argc, char** argv, std::string configFileName, bool ignoreUnknown = false);

	static void readFromFile(boost::filesystem::path configFile, boost::program_options::variables_map& values);

	static std::set<program_option_impl*, is_less>* Options;

	static std::set<std::string>* KnownLongParams;

	static std::map<const program_option_impl*, std::string> Values;

	static boost::program_options::options_description* CommandLineOptions;

	static boost::program_options::positional_options_description* Positional;

	static boost::program_options::options_description* ConfigFileOptions;

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
		_defaultValue(boost::lexical_cast<std::string>(args[_default_value | "0"])),
		_isPositional(args[_is_positional | false]) {

		ProgramOptions::addProgramOption(this); 
	}

	std::string getModuleName() const;

	std::string getLongParam() const;

	std::string getShortParam() const;

	std::string getDescription() const;

	std::string getArgumentSketch() const;

	std::string getDefaultValue() const;

	bool isPositional() const;

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
	 * as long as the argument given to fooOption can be converted to int
	 * (otherwise, an exception will be thrown).
	 *
	 * This conversion also allows to check if an option is set. The implicit
	 * conversion to bool ensures:
	 *
	 *   if (fooOption) {
	 *
	 *     // fooOption is set to a value different to 'false'
	 *
	 *   } else {
	 *
	 *     // fooOption was not set or set to 'false'
	 *   }
	 */
	template <typename ValueType>
	operator ValueType() const {

		OptionConverter<ValueType> converter;

		return converter(*this);
	}

	/**
	 * Like type converter above, but to be used in boolean expressions.
	 */
	explicit operator bool() const {

		// option is not set
		if (!ProgramOptions::isOptionSet(*this)) {

			try {

				// is its default "true"?
				if (boost::lexical_cast<bool>(ProgramOptions::getOptionValue(*this)))
					return true;

			} catch (boost::bad_lexical_cast&) {

				UTIL_THROW_EXCEPTION(
						UsageError,
						"program option " << getLongParam() <<
						" with value " << ProgramOptions::getOptionValue(*this) <<
						" can not be (lexically) cast into bool");
			}

			// otherwise, we say false
			return false;
		}

		if (ProgramOptions::getOptionValue(*this) == "false")
			return false;

		return true;
	}

	/**
	 * Explicit conversion to ValueType.
	 */
	template <typename ValueType>
	ValueType as() {

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

	bool _isPositional;
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

/**
 * Default type conversion for options. Tries to cast the value string to the
 * desired type.
 */
template <typename TargetType>
struct OptionConverter {

	TargetType operator()(const program_option_impl& option) const {

		try {

			return boost::lexical_cast<TargetType>(ProgramOptions::getOptionValue(option));

		} catch (boost::bad_lexical_cast&) {

			UTIL_THROW_EXCEPTION(
					UsageError,
					"program option " << option.getLongParam() <<
					" with value " << ProgramOptions::getOptionValue(option) <<
					" can not be (lexically) cast into " << typeName(TargetType()));
		}
	}
};

/**
 * Template specialisation for conversions to bool. Returns true, iff the
 * option is set and its value is not 'false'.
 */
template <>
struct OptionConverter<bool> {

	bool operator()(const program_option_impl& option) const {

		return static_cast<bool>(option);
	}
};

struct ProgramOptionsCleanup {

	~ProgramOptionsCleanup() {

		if (ProgramOptions::Options != 0)
			delete ProgramOptions::Options;

		if (ProgramOptions::KnownLongParams != 0)
			delete ProgramOptions::KnownLongParams;

		if (ProgramOptions::CommandLineOptions != 0)
			delete ProgramOptions::CommandLineOptions;

		if (ProgramOptions::Positional != 0)
			delete ProgramOptions::Positional;

		if (ProgramOptions::ConfigFileOptions  != 0)
			delete ProgramOptions::ConfigFileOptions;
	}
};

extern ProgramOptionsCleanup programOptionsCleanup;

std::ostream&
operator<<(std::ostream& os, program_option_impl& option);

} // namespace util

#endif /*COMMANDLINEPARSER_H_*/
