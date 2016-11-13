#include "demangle.h"
#ifndef _MSC_VER
#include <cxxabi.h>
#endif

std::string demangle(const char* name) {

	int status = -4;

#ifdef _MSC_VER
	std::string result(name);
#else
	char* demangled = abi::__cxa_demangle(name, 0, 0, &status);
	std::string result = (status == 0 ? demangled : name);
	free(demangled);
#endif

	return result;
}
