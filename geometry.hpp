#ifndef UTIL_GEOMETRY_H__
#define UTIL_GEOMETRY_H__

#include "ray.hpp"

namespace util {

/**
 * Rotate a point around a ray by the given number of degrees.
 */
template <typename T>
point<T,3> rotate3d(const ray<T,3>& ray, T degrees, point<T,3> point) {

	// move starting point of ray to origin
	point -= ray.position();

	util::point<T,3> axis = ray.direction();
	T length = sqrt(axis.x()*axis.x() + axis.y()*axis.y() + axis.z()*axis.z());
	axis /= length;

	// axis x point
	util::point<T,3> cross;
	cross.x() = axis.y()*point.z() - axis.z()*point.y();
	cross.y() = axis.z()*point.x() - axis.x()*point.z();
	cross.z() = axis.x()*point.y() - axis.y()*point.x();

	// axis * point
	T dot = axis.x()*point.x() + axis.y()*point.y() + axis.z()*point.z();

	T cosDeg = cos(degrees);
	T sinDeg = sin(degrees);

	// rotate around axis direction
	point = cosDeg*point + sinDeg*(cross) + (1 - cosDeg)*dot*axis;

	// move back
	point += ray.position();

	return point;
}

} // namespace util

#endif // UTIL_GEOMETRY_H__

