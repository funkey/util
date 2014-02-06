#ifndef UTIL_FILES_H__
#define UTIL_FILES_H__

#include <string>
#include <fstream>

#define UTIL_READLINE_BUFFER_SIZE 256

/**
 * Read a whole line from an ifstream and return it as a string.
 */
std::string readline(std::ifstream& s);

#endif // UTIL_FILES_H__

