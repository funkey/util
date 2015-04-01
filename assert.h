#ifndef UTIL_ASSERT_H__
#define UTIL_ASSERT_H__

#include "exceptions.h"

namespace util {

class AssertionFailed : public Exception {};

}

#ifdef NDEBUG

#define UTIL_ASSERT(expr)                (void(0))
#define UTIL_ASSERT_REL(left, op, right) (void(0))

#else // DEBUG

#define UTIL_ASSERT(expr)                if (!(expr))          { \
	UTIL_THROW_EXCEPTION(                                        \
			util::AssertionFailed,                               \
			"\"" << #expr << "\" failed in " <<                  \
			__func__ << ", " << __FILE__ << ":" << __LINE__);    \
}

#define UTIL_ASSERT_REL(left, op, right) if (!(left op right)) {                \
	UTIL_THROW_EXCEPTION(                                                       \
			util::AssertionFailed,                                                    \
			"\"" << #left << " " << #op << " " << #right << "\" ==> " <<        \
			left << " " << #op << " " << right <<                               \
			" failed in " << __func__ << ", " << __FILE__ << ":" << __LINE__);  \
}

#endif

#endif // UTIL_ASSERT_H__

