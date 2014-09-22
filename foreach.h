#ifndef UTIL_FOREACH_H__
#define UTIL_FOREACH_H__

#include <boost/foreach.hpp>
#include <boost/version.hpp>

namespace boost {

#if BOOST_VERSION > 104900
	namespace BOOST_FOREACH = foreach;
#endif

} // namespace boost

#define foreach BOOST_FOREACH

#endif // UTIL_FOREACH_H__

