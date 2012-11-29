#ifndef UTIL_DEMANGLE_H__
#define UTIL_DEMANGLE_H__

#include <cxxabi.h>
#include <cstdlib>
#include <string>

namespace {

std::string demangle(const char* name) {

	int status = -4;

	char* demangled = abi::__cxa_demangle(name, 0, 0, &status);

	std::string result = (status == 0 ? demangled : name);

	free(demangled);

	return result;
}

}

#endif // UTIL_DEMANGLE_H__

