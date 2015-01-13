#ifndef UTIL_ASSERT_H__
#define UTIL_ASSERT_H__

#include "exceptions.h"

namespace util {

class AssertionFailed : public Exception {};

inline void assert_failed(const char* expr, const char* func, int line, const char* file) {

	UTIL_THROW_EXCEPTION(
			AssertionFailed,
			"\"" << expr << "\" failed in " << func << ", " << file << ":" << line);
}

template <typename L, typename R>
void assert_rel_failed(
		const char* left,
		const char* op,
		const char* right,
		const L&    l,
		const R&    r,
		const char* func,
		int line,
		const char* file) {

	UTIL_THROW_EXCEPTION(
			AssertionFailed,
			"\"" << left << " " << op << " " << right << "\" ==> " <<
			l << " " << op << " " << r <<
			" failed in " << func << ", " << file << ":" << line);
}

}

#ifdef NDEBUG

#define UTIL_ASSERT(expr)                (void(0))
#define UTIL_ASSERT_REL(left, op, right) (void(0))

#else // DEBUG

#define UTIL_ASSERT(expr)                if (!(expr))          { ::util::assert_failed(#expr, __func__, __LINE__, __FILE__); }
#define UTIL_ASSERT_REL(left, op, right) if (!(left op right)) { ::util::assert_rel_failed(#left, #op, #right, left, right, __func__, __LINE__, __FILE__); }

#endif

#endif // UTIL_ASSERT_H__

