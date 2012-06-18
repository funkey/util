/*
* $URL$
* $Rev$
* $Author: s2946182 $
* $Date: 2008/01/15 15:41:13 $
* $Id: Logger.cc,v 1.3 2008/01/15 15:41:13 s2946182 Exp $
*/

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

#include <boost/thread.hpp>

#include "ProgramOptions.h"
#include "Logger.h"

namespace logger {

// Define command line options
util::ProgramOption logLevel("", "--log-level", "-v",
    "Specifies the level of verbosity. "
    "Valid values for level are: \"none\" (no ouput at all), "
    "\"error\" (error messages only), "
    "\"user\" (useful information, default setting), "
    "\"debug\" (internal information used for debugging) "
    "and \"all\" (maximum verbosity).\n"
    "See section on logging for more options.", "level");

util::ProgramOption showChannels("Logging", "--show-log-channels", "",
    "Prints all avalaible log-channels. Log-channels are used to "
    "collect logging messages belonging to certain units of the "
    "program so that they can be adjusted individually. If "
    "you leave everything as it is, log-channel messages are handled "
    "like any other log message the program.");

util::ProgramOption channelLevel("Logging", "--log-level-c", "-vc",
    "Sets the log level for certain log-channels independent "
    "of the global log level.\n"
    "levels syntax: channel1=loglevel1,channel2=loglevel2,...\n"
    "For a list of possible channels type "
    "\"--show-log-channels\".", "levels");

util::ProgramOption channelToFile("Logging", "--log-file-c", "",
    "Redirects the output of certain log-channels to files.\n"
    "files syntax: channel1=file1,channel2=file2,...\n"
    "For a list of possible channels type "
    "\"--show-log-channels\".", "files");

Logger glutton(0);

// Initialize global logging-streams:
LogLevel  error = Error;
LogLevel  user  = User;
LogLevel  debug = Debug;
LogLevel  all   = All;

LogChannel  out("default");

// Declare static objects
std::map<std::string, std::filebuf*>  LogFileManager::filebuffers;
LogLevel                              LogManager::globalLogLevel = User;
LogFileManager                        LogChannel::logFileManager;
std::set<LogChannel*>*                LogChannel::logChannels = 0;


// Implementation - LogChannel

LogChannel::LogChannel(std::string channelName, std::string prefix) :
  _channelName(channelName),
  _prefix(prefix),
  _error(std::cerr.rdbuf()),
  _user(std::cout.rdbuf()),
  _debug(std::cout.rdbuf()),
  _all(std::cout.rdbuf()),
  _level(Global)
{
  getChannels()->insert(this);
}

std::set<LogChannel*>*
LogChannel::getChannels() {

  if (logChannels == 0)
    logChannels = new std::set<LogChannel*>();

  return logChannels;
}

std::ostream&
LogChannel::operator()(LogLevel level) {

  LogLevel  myLevel =
    (_level == Global ?
      LogManager::getGlobalLogLevel() : _level);

  switch (level) {
    case Error:
      if (myLevel >= Error)
        return _error << boost::this_thread::get_id() << " " << _prefix;
      break;
    case User:
      if (myLevel >= User)
        return _user << boost::this_thread::get_id() << " " << _prefix;
      break;
    case Debug:
      if (myLevel >= Debug)
        return _debug << boost::this_thread::get_id() << " " << _prefix;
      break;
    case All:
      if (myLevel >= All)
        return _all << boost::this_thread::get_id() << " " << _prefix;
      break;
    default:
      break;
  }
  return glutton;
}

void
LogChannel::setLogLevel(LogLevel level) {

  _level = level;
}

const LogLevel&
LogChannel::getLogLevel() {

  if (_level == Global)
    return LogManager::getGlobalLogLevel();

  return _level;
}

void
LogChannel::redirectToFile(std::string filename) {

  Logger fileLogger(LogFileManager::openFile(filename));

  _error = fileLogger;
  _user  = fileLogger;
  _debug = fileLogger;
  _all   = fileLogger;
}

// Implementation - LogFileManager

LogFileManager::~LogFileManager() {

  std::map<std::string, std::filebuf*>::iterator i;

  for (i = filebuffers.begin(); i !=  filebuffers.end(); i++) {
    (*i).second->close();
    delete (*i).second;
  }
}

std::filebuf*
LogFileManager::openFile(std::string filename) {

  if (filebuffers.find(filename) != filebuffers.end())
    return filebuffers[filename];

  std::filebuf* filebuffer = new std::filebuf();
  if (!filebuffer->open(filename.c_str(), std::ios_base::out)) {

    LOG_ERROR(out) << "[LogFileManager] Unable to open \"" << filename << "\".\n";
    throw "[LogFileManager] Attempt to open file \"" + filename + "\" failed.";
  }
  filebuffers[filename] = filebuffer;
  return filebuffer;
}


// Implementation - Logger

Logger::Logger(std::streambuf* streamBuffer) :
  std::ostream(streamBuffer)
{
  // Empty
}

Logger::Logger(Logger& logger) :
  std::ostream(logger.rdbuf()) {
  // Empty
}

Logger&
Logger::operator=(const Logger& logger) {

  rdbuf(logger.rdbuf());
  return *this;
}

void
LogManager::init()
{
  // set global log level
  if (!util::ProgramOptions::isOptionSet(logLevel))
  {
    setGlobalLogLevel(User);
    out.setLogLevel(User);
  } 
  else
  {
    LOG_DEBUG(out) << "[LogManager] " << logLevel.getLongParam() << " command-line option detected\n";

    std::string verbosity =
      util::ProgramOptions::getOptionValue(logLevel);

    setGlobalLogLevel(getLogLevel(verbosity));

    LOG_DEBUG(out) << "[LogManager] Loglevel set to \"" << verbosity << "\"\n";
  }

  if (util::ProgramOptions::isOptionSet(showChannels)) {
    printChannels();
    exit(0);
  }

  // set channel log levels
  if (util::ProgramOptions::isOptionSet(channelLevel)) {
    
    LOG_DEBUG(out) << "[LogManager] " << channelLevel.getLongParam() << " command-line option detected\n";

    std::string channelLevels = util::ProgramOptions::getOptionValue(channelLevel);

    while (channelLevels.length() > 0) {

      size_t pos_eq = channelLevels.find_first_of("=");
      LOG_ALL(out) << "[LogManager] Found '=' at " << pos_eq << "\n";
      if (pos_eq == std::string::npos) {
        LOG_ERROR(out) << "[LogManager] Invalid " << channelLevel.getLongParam() << " syntax.\n";
        util::ProgramOptions::printUsage();
        throw "[LogManager] Invalid command line syntax";
      }
      std::string name = channelLevels.substr(0, pos_eq);

      channelLevels = channelLevels.substr(pos_eq + 1, channelLevels.length() - 1);

      std::string level;
      size_t pos_co = channelLevels.find_first_of(",");
      if (pos_co == std::string::npos) {
        LOG_ALL(out) << "[LogManager] no further channels" << "\n";
        level = channelLevels.substr(0, channelLevels.length());
        channelLevels = "";
      } else {
        LOG_ALL(out) << "[LogManager] ',' detected, there are further channels at " << pos_co << "\n";
        level = channelLevels.substr(0, pos_co);
        channelLevels = channelLevels.substr(pos_co + 1, channelLevels.length() - 1);
        LOG_ALL(out) << "[LogManager] remaining argument " << channelLevels << "\n";
      }

      std::set<LogChannel*> channels = getChannels(name);
      for (channel_it i = channels.begin(); i != channels.end(); i++)
          (*i)->setLogLevel(getLogLevel(level));

      LOG_DEBUG(out) << "[LogManager] log-level of channel \"" << name
                     << "\" set to \"" << level << "\"\n";
    }
  }

  // redirect channels to files
  if (util::ProgramOptions::isOptionSet(channelToFile)) {

   LOG_DEBUG(out) << "[LogManager] " << channelToFile.getLongParam() << " command-line option detected\n";

    std::string channelFiles = util::ProgramOptions::getOptionValue(channelToFile);

    while (channelFiles.length() > 0) {

      size_t pos_eq = channelFiles.find_first_of("=");
      if (pos_eq == std::string::npos) {
        LOG_ERROR(out) << "[LogManager] Invalid " << channelToFile.getLongParam() << " syntax.\n";
        util::ProgramOptions::printUsage();
        throw "[LogManager] Invalid command line syntax";
      }
      std::string name = channelFiles.substr(0, pos_eq);

      channelFiles = channelFiles.substr(pos_eq + 1, channelFiles.length() - 1);

      std::string file;
      size_t pos_co = channelFiles.find_first_of(",");
      if (pos_co == std::string::npos) {
        file = channelFiles.substr(0, channelFiles.length());
        channelFiles = "";
      } else {
        file = channelFiles.substr(0, pos_co);
        channelFiles = channelFiles.substr(pos_co + 1, channelFiles.length() - 1);
      }

      std::set<LogChannel*> channels = getChannels(name);
      for (channel_it i = channels.begin(); i != channels.end(); i++)
          (*i)->redirectToFile(file);

      LOG_DEBUG(out) << "[LogManager] channel \"" << name
                     << "\" redirected to file \"" << file << "\"\n";
    }
 }
}

void
LogManager::setGlobalLogLevel(LogLevel level) {

  globalLogLevel = level;
}

const LogLevel&
LogManager::getGlobalLogLevel() {

  return globalLogLevel;
}

LogLevel
LogManager::getLogLevel(std::string strLevel) {

    if (strLevel == "all")
      return All;
    else if (strLevel == "user")
      return User;
    else if (strLevel == "debug")
      return Debug;
    else if (strLevel == "error")
      return Error;
    else if (strLevel == "none")
      return Quiet;
    else {
      LOG_ERROR(out) << "[Logger] Unknown log level \"" << strLevel << "\".\n";
      util::ProgramOptions::printUsage();
      throw "[Logger] Invalid log level";
    }
}

std::set<LogChannel*>
LogManager::getChannels(std::string channelName) {

  std::set<LogChannel*> channels;

  for (channel_it i = LogChannel::getChannels()->begin();
       i != LogChannel::getChannels()->end(); i++) {
    if ( (*i)->getName() == channelName)
      channels.insert(*i);
  }

  if (channels.size() > 0)
    return channels;

  LOG_ERROR(out) << "[LogManager] No channel \"" << channelName << "\" available.\n";
  printChannels();
  throw "[LogManager] Invalid channel name";
}

void
LogManager::printChannels() {

  if (LogChannel::getChannels()->size() == 0) {
    LOG_USER(out) << "No output channels for this application available.\n";
    return;
  }

  std::string prevChannelName = "";

  LOG_USER(out) << "\nValid output channels are:\n\t";
  for (channel_it i = LogChannel::getChannels()->begin();
       i != LogChannel::getChannels()->end(); i++) {
    if ((*i)->getName() != prevChannelName) {
      LOG_USER(out) << (*i)->getName() << " ";
      prevChannelName = (*i)->getName();
    }
  }
  LOG_USER(out) << "\n\n";
}

} // namespace logger

// End of file
