#ifndef YANTA_UTIL_TORUS_MAPPING_H__
#define YANTA_UTIL_TORUS_MAPPING_H__

#include <util/point.hpp>
#include <util/box.hpp>

#include "ring_mapping.hpp"

/**
 * Provides a mapping from logical coordinates in a 2D interval of size wxh to 
 * physical coordinates in [0,w)x[0,h). When the logical interval of gets 
 * shifted, most of the mapping stays as it is, only the boundary of the mapping 
 * changes, thus forming a torus.
 */
template <typename Precision, int W = util::detail::Dynamic, int H = util::detail::Dynamic>
class torus_mapping {

public:

	/**
	 * Create a new mapping from [0,w)x[0,h) to [0,w)x[0,h).
	 */
	torus_mapping(Precision w = util::detail::Dynamic, Precision h = util::detail::Dynamic) :
		_xring(w),
		_yring(h) {}

	/**
	 * Get the logical region represented by this mapping.
	 */
	inline util::box<Precision,2> get_region() {

		Precision bx, by, ex, ey;

		_xring.get_interval(bx, ex);
		_yring.get_interval(by, ey);

		return util::box<Precision,2>(bx, by, ex, ey);
	}

	/**
	 * Map a logical point p to its physical coordinats.
	 */
	inline util::point<Precision,2> map(const util::point<Precision,2>& p) {

		return util::point<Precision,2>(_xring.map(p.x()), _yring.map(p.y()));
	}

	/**
	 * Shift the interval represented by this torus by s.
	 */
	void shift(const util::point<Precision,2>& s) {

		_xring.shift(s.x());
		_yring.shift(s.y());
	}

	/**
	 * Reset the mapping, such that [begin.x(),begin.x()+w)x[begin.y(),begin.y()+h) maps 
	 * to [0,w)x[0,h).
	 */
	void reset(const util::point<Precision,2>& begin) {

		_xring.reset(begin.x());
		_yring.reset(begin.y());
	}

	/**
	 * Split a logical 2D interval into four logical intervals, such that the 
	 * mapping in these intervals is linear (i.e., does not wrap around the 
	 * torus boundaries).
	 *
	 * Up to three of the intervals can be empyt, in which case it is set to 
	 * [0,0,0,0).
	 *
	 * @param region
	 *              The 2D interval to split.
	 * @param subregions [out]
	 *              Start of an array of four rectangles to be filled with the 
	 *              four subintervals.
	 */
	void split(const util::box<Precision,2>& region, util::box<Precision,2>* subregions) {

		Precision xbegins[2];
		Precision xends[2];
		Precision ybegins[2];
		Precision yends[2];

		_xring.split(region.min().x(), region.max().x(), xbegins, xends);
		_yring.split(region.min().y(), region.max().y(), ybegins, yends);

		subregions[0].min().x() = xbegins[0];
		subregions[0].min().y() = ybegins[0];
		subregions[0].max().x() = xends[0];
		subregions[0].max().y() = yends[0];

		subregions[1].min().x() = xbegins[1];
		subregions[1].min().y() = ybegins[0];
		subregions[1].max().x() = xends[1];
		subregions[1].max().y() = yends[0];

		subregions[2].min().x() = xbegins[0];
		subregions[2].min().y() = ybegins[1];
		subregions[2].max().x() = xends[0];
		subregions[2].max().y() = yends[1];

		subregions[3].min().x() = xbegins[1];
		subregions[3].min().y() = ybegins[1];
		subregions[3].max().x() = xends[1];
		subregions[3].max().y() = yends[1];

		for (int i = 0; i < 4; i++)
			if (subregions[i].area() <= 0) {

				subregions[i].min().x() = 0;
				subregions[i].max().x() = 0;
				subregions[i].min().y() = 0;
				subregions[i].max().y() = 0;
			}
	}

private:

	ring_mapping<Precision, W> _xring;
	ring_mapping<Precision, H> _yring;
};

#endif // YANTA_UTIL_TORUS_MAPPING_H__

