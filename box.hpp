#ifndef UTIL_BOX_H__
#define UTIL_BOX_H__

#include "point3.hpp"
#include "rect.hpp"

namespace util {

/**
 * Represents a box with a minimal point (inclusive) and a maximal point 
 * (exclusive).
 */
template <typename T>
struct box {

	util::point3<T> min;
	util::point3<T> max;

	/**
	 * Default constructor.
	 */
	box() {};

	template <typename S>
	box(const box<S>& other) :
		min(other.min),
		max(other.max) {};

	template <typename S>
	box(const point3<S>& min_, const point3<S>& max_) :
		min(min_),
		max(max_) {}

	box(const T& minX_, const T& minY_, const T& minZ_, const T& maxX_, const T& maxY_, const T& maxZ_) :
		min(minX_, minY_, minZ_),
		max(maxX_, maxY_, maxZ_) {};

	T width() const {

		return max.x - min.x;
	}

	T height() const {

		return max.y - min.y;
	}

	T depth() const {

		return max.z - min.z;
	}

	point3<T> size() const {

		return max - min;
	}

	bool contains(const point3<T>& point) const {

		return
				min.x <= point.x && min.y <= point.y && min.z <= point.z &&
				max.x >  point.x && max.y >  point.y && max.z >  point.z;
	}

	bool contains(const box<T>& other) const {

		return 
				min.x <= other.min.x && min.y <= other.min.y && min.z <= other.min.z &&
				max.x >= other.max.x && max.y >= other.max.y && max.z >= other.max.z;
	}

	/**
	 * Extend this box, such that it fits the given point3.
	 */
	void fit(const point3<T>& point) {

		min.x = std::min(point.x, min.x);
		min.y = std::min(point.y, min.y);
		min.z = std::min(point.z, min.z);
		max.x = std::max(point.x, max.x);
		max.y = std::max(point.y, max.y);
		max.z = std::max(point.z, max.z);
	}

	/**
	 * Extend this box, such that it fits the given box.
	 */
	void fit(const box<T>& box) {

		min.x = std::min(box.min.x, min.x);
		min.y = std::min(box.min.y, min.y);
		min.z = std::min(box.min.z, min.z);
		max.x = std::max(box.max.x, max.x);
		max.y = std::max(box.max.y, max.y);
		max.z = std::max(box.max.z, max.z);
	}

	T volume() const {

		return width()*height()*depth();
	}

	/**
	 * Get a rectangle that corresponds to the x- and y-extents of this box.
	 */
	rect<T> project_xy() const {

		return rect<T>(min.x, min.y, max.x, max.y);
	}
};

} // namespace util

template <typename T>
std::ostream& operator<<(std::ostream& os, const util::box<T>& box) {

	os << "[" << box.min.x << ", " << box.min.x << ", " << box.min.z
	   << "; " << box.max.x << ", " << box.max.y << ", " << box.max.z << "]";

	return os;
}

#endif // UTIL_BOX_H__

