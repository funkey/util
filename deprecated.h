#ifndef UTIL_DEPRECATED_H__
#define UTIL_DEPRECATED_H__

#ifdef __GNUC__
#define DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED(func) __declspec(deprecated) func
#else
#pragma message("This compiler is not supported for the DEPRECATED macro")
#define DEPRECATED(func) func
#endif

#endif // UTIL_DEPRECATED_H__

