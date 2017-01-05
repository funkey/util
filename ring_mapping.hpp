#ifndef YANTA_UTIL_RING_MAPPING_H__
#define YANTA_UTIL_RING_MAPPING_H__

#include <cassert>

namespace util {
namespace detail {

const int Dynamic = -1;

/**
 * Template base class for statically sized ring mappings.
 */
template <typename Precision, int L>
class ring_mapping_base {

public:

	ring_mapping_base(Precision) {}

	Precision length() { return L; }
};

/**
 * Template specialization for dynamically sized ring mappings.
 */
template <typename Precision>
class ring_mapping_base<Precision, Dynamic> {

public:

	ring_mapping_base(Precision l) : _length(l) {}

	Precision length() { return _length; }

private:

	Precision _length;
};

/**
 * Compute the modulo the way we want it.
 */
template <typename Precision>
Precision mod(Precision a, Precision b) {

	Precision c = a % b;

	return (c < 0 ? c + b : c);
}

} // namespace detail
} // namespace util

/**
 * Provides a mapping from logical coordinates in an interval of size l to 
 * physical coordinates in [0,l). When the logical interval of gets shifted, 
 * most of the mapping stays as it is, only the boundary of the mapping changes, 
 * thus forming a ring.
 *
 * Example, starting with a ring mapping representing the interval [0,l):
 *
 *   physical:  0 1 2 3 .... l-1
 *              |--------------|
 *   logical :  0 1 2 3 .... l-1
 *
 * After a shift by +1, the represented interval is [1,l+1):
 *
 *   physical:  0 1 2 3 .... l-1
 *              |--------------|
 *   logical :  l 1 2 3 .... l-1
 */
template <typename Precision, int L = util::detail::Dynamic>
class ring_mapping : public util::detail::ring_mapping_base<Precision, L> {

public:

	/**
	 * Create a new mapping from [0,size) to [0,size).
	 */
	ring_mapping(unsigned int size = L) :
		util::detail::ring_mapping_base<Precision, L>(size),
		_start(0),
		_physicalStart(0),
		_split(0) {}

	using util::detail::ring_mapping_base<Precision, L>::length;

	/**
	 * Get the logical interval represented by this mapping.
	 */
	inline void get_interval(Precision& begin, Precision& end) {

		begin = _start;
		end   = _start + length();
	}

	/**
	 * Map a logical coordinate x to its physical coordinate.
	 */
	Precision map(Precision x) {

		assert(x >= _start);
		assert(x <  _start + length());

		return util::detail::mod(_physicalStart + x - _start, length());
	}

	/**
	 * Shift the logical interval by s.
	 */
	void shift(Precision s) {

		_start += s;
		_physicalStart = util::detail::mod(_physicalStart + s, length());
		_split = _start + util::detail::mod(length() - _physicalStart, length());
	}

	/**
	 * Reset the mapping, such that [begin, begin+l) maps to [0,l).
	 */
	void reset(Precision begin) {

		_start = begin;
		_physicalStart = 0;
		_split = begin;
	}

	/**
	 * Split a logical interval [begin, end) into two logical intervals [begin, 
	 * split), [split, end), such that the mapping in these two intervals is 
	 * linear (i.e., does not cross the end of the physical mapping).
	 *
	 * One of the intervals can be empyt, in which case it is set to [0,0).
	 *
	 * @param begin
	 *              Begin of the interval to split.
	 * @param end
	 *              End of the interval to split.
	 * @param begins [out]
	 *              Begins of the two subintervals. Start of array of two 
	 *              elements.
	 * @param ends [out]
	 *              Ends of the two subintervals. Start of array of two 
	 *              elements.
	 */
	void split(Precision begin, Precision end, Precision* begins, Precision* ends) {

		begins[0] = begin;
		ends[0]   = _split;

		begins[1] = _split;
		ends[1]   = end;

		for (int i = 0; i < 2; i++)
			if (ends[i] <= begins[i]) {

				begins[i] = 0;
				ends[i]   = 0;
			}
	}

private:

	// the start of the interval in logical coordinates
	Precision _start;

	// the start of the interval in physical coordinates
	Precision _physicalStart;

	// the split point of the interval in logical coordinates
	Precision _split;
};

#endif // YANTA_UTIL_RING_MAPPING_H__

