/*
* $URL$
* $Rev$
* $Author: s2946182 $
* $Date: 2007/11/12 15:03:42 $
* $Id: Logger.hh,v 1.1 2007/11/12 15:03:42 s2946182 Exp $
*/

#ifndef __LOGGER_HH
#define __LOGGER_HH

#include<iostream>
#include<string>
#include<vector>
#include<set>
#include<map>

#include <boost/thread.hpp>

#define LOG_ERROR(channel) if (channel.getLogLevel() >= logger::Error) channel(logger::error)
#define LOG_USER(channel)  if (channel.getLogLevel() >= logger::User)  channel(logger::user)
#define LOG_DEBUG(channel) if (channel.getLogLevel() >= logger::Debug) channel(logger::debug)
#define LOG_ALL(channel)   if (channel.getLogLevel() >= logger::All)   channel(logger::all)

namespace logger {

enum LogLevel
{
  Global = 0,
  Quiet,    // no output at all      - no errors either
  Error,    // print errors only     - no user output
  User,     // output for the user   - most suitable setting
  Debug,    // output for developer  - give detailed information
            //                         about what's going on
  All       // maximum verbosity     - everything that's to much
            //                         for debug-output
};

// forward declaration
class LogChannel;

class Logger : public std::ostream {

public:

	Logger(std::streambuf* streamBuffer, const std::string& prefix);

	Logger(Logger& logger, const std::string& prefix);

	Logger& operator=(const Logger& logger);

	template <typename T>
	Logger& operator<<(T* t) {

		printPrefix();

		std::ostream& s = *this;
		s << t;

		return *this;
	}

	template <typename T>
	Logger& operator<<(const T& t) {

		printPrefix();

		std::ostream& s = *this;
		s << t;

		return *this;
	}

	Logger& operator<<(std::ostream&(*fp)(std::ostream&)) {

		printPrefix();

		std::ostream::operator<<(fp);

		// the next operator<< will cause the prefix to be printed after a 
		// newline
		if (fp == &std::endl<std::ostream::char_type, std::ostream::traits_type>)
			_printPrefix = true;

		return *this;
	}

	Logger& operator<<(std::ios&(*fp)(std::ios&)) {

		printPrefix();

		std::ostream::operator<<(fp);

		return *this;
	}

	Logger& operator<<(std::ios_base&(*fp)(std::ios_base&)) {

		printPrefix();

		std::ostream::operator<<(fp);

		return *this;
	}

private:

	void printPrefix() {

		if (_printPrefix) {

			_printPrefix = false;
			*this << boost::this_thread::get_id() << " " << _prefix;
		}
	}

	// reference to the owning LogChannel's prefix
	const std::string& _prefix;

	bool _printPrefix;
};

class LogFileManager {

  public:

    ~LogFileManager();

    static std::filebuf* openFile(std::string filename);

  private:

    static std::map<std::string, std::filebuf*> filebuffers;

    static LogLevel getLogLevel(std::string loglevel);

    static LogChannel* getChannel(std::string name);

    static void printChains();

    static LogLevel  globalLogLevel;
};

class LogChannel {

  public:

    static std::set<LogChannel*>*  getChannels();

    LogChannel(std::string channelName, std::string prefix = "");

    Logger& operator()(LogLevel level);

    std::string getName() { return _channelName; };

    void  setLogLevel(LogLevel level);

    const LogLevel& getLogLevel();

    void  redirectToFile(std::string filename);

  private:

    static  std::set<LogChannel*>*  logChannels;

    static  LogFileManager  logFileManager;

    std::string _channelName;

    std::string _prefix;

    Logger  _error;

    Logger  _user;

    Logger  _debug;

    Logger  _all;

    LogLevel  _level;

    bool  _writeToFile;
};

class LogManager {

  public:

    static void init();

    static void setGlobalLogLevel(LogLevel logLevel);

    static const LogLevel& getGlobalLogLevel();

  private:

    static void setChainLogLevel(LogChannel* chain, LogLevel level);

    static void redirectChainToFile(LogChannel* chain, std::string filename);

    static LogLevel getLogLevel(std::string level);

    static std::set<LogChannel*> getChannels(std::string channelName);

    static void printChannels();


    static LogLevel globalLogLevel;
};

typedef std::set<LogChannel*>::iterator channel_it;

extern LogChannel out;

extern LogLevel error;
extern LogLevel user;
extern LogLevel debug;
extern LogLevel all;

} // namespace logger

#endif  // #ifndef __LOGGER_HH
