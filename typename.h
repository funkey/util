#ifndef TYPENAME_H__
#define TYPENAME_H__

#include <cxxabi.h>
#include <cstdlib>
#include <typeinfo>
#include <string>

namespace {

std::string demangle(const char* name) {

	int status = -4;

	char* demangled = abi::__cxa_demangle(name, 0, 0, &status);

	std::string result = (status == 0 ? demangled : name);

	free(demangled);

	return result;
}

template <typename T>
std::string typeName(T* p) {

	return demangle(typeid(*p).name());
}

template <typename T>
std::string typeName(const T& p) {

	return demangle(typeid(p).name());
}

}

#endif // TYPENAME_H__

