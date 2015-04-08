#ifndef UTIL_GEOMETRY_H__
#define UTIL_GEOMETRY_H__

#include "ray.hpp"
#include "plane.hpp"

namespace util {

/**
 * Dot product between two points.
 */
template <typename T, int N>
T dot(const util::point<T,N>& a, const util::point<T,N>& b) {

	T sum = 0;
	for (int i = 0; i < N; i++)
		sum += a[i]*b[i];
	return sum;
}

/**
 * Cross product (only for N=3).
 */
template <typename T>
util::point<T,3> cross(const util::point<T,3>& a, const util::point<T,3>& b) {

	return util::point<T,3>(
			a.y()*b.z() - a.z()*b.y(),
			a.z()*b.x() - a.x()*b.z(),
			a.x()*b.y() - a.y()*b.x());
}

/**
 * Euclidean length of a vector.
 */
template <typename T, int N>
T length(const util::point<T,N>& p) {

	return sqrt(dot(p, p));
}

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
	util::point<T,3> c = cross(axis, point);

	// axis * point
	T d = dot(axis, point);

	T cosDeg = cos(degrees);
	T sinDeg = sin(degrees);

	// rotate around axis direction
	point = cosDeg*point + sinDeg*c + (1 - cosDeg)*d*axis;

	// move back
	point += ray.position();

	return point;
}

/**
 * Find the intersection position of a ray with a plane. The return value is 
 * true, if the ray intersects the plane. In this case, parameter t is set such 
 * that ray.position() + t*ray.direction() is the intersection point.
 */
template <typename T, int N>
bool intersect(const plane<T,N>& p, const ray<T,N>& r, T& t) {

	T dot_rp = dot(r.direction(), p.normal());

	if (dot_rp == 0)
		return false;

	t = dot(p.position() - r.position(), p.normal())/dot_rp;

	return true;
}

} // namespace util

#endif // UTIL_GEOMETRY_H__

