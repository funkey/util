#ifndef UTIL_GEOMETRY_H__
#define UTIL_GEOMETRY_H__

#include "ray.hpp"
#include "plane.hpp"
#include "box.hpp"

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

/**
 * Find the intersection position of a ray with a box. The return value is true, 
 * if the ray intersects the box. In this case, parameter t is set such that 
 * ray.position() + t*ray.direction() is the closest intersection point 
 * (smallest t).
 */
template <typename T, int N>
bool intersect(const box<T,N>& b, const ray<T,N>& r, T& t) {

	bool intersects = false;

	for (int d = 0; d < N; d++) {

		point<T,N> normal;
		normal[d] = 1;

		plane<T,N> facePlanes[2] = {
				plane<T,N>(b.min(), normal),
				plane<T,N>(b.max(), normal)
		};

		T t_;
		for (int i = 0; i < 2; i++) {
			if (intersect(facePlanes[i], r, t_)) {

				bool onFace = true;
				util::point<T,N> p = r.at(t_);

				for (unsigned int td = 0; td < N; td++) {
					if (td == d)
						continue;

					if (p[td] < b.min()[td] || p[td] >= b.max()[td]) {
						onFace = false;
						break;
					}
				}

				if (onFace) {
					intersects = true;
					t = std::min(t, t_);
				}
			}
		}
	}

	return intersects;
}

/**
 * Compute the distance between two rays. s and t will be set with the positions 
 * of the closest points.
 */
template <typename T>
T distance(const ray<T,3>& ray1, const ray<T,3>& ray2, T& s, T& t) {

	util::point<T,3> uv  = cross(ray1.direction(), ray2.direction());
	util::point<T,3> a_b = ray1.position() - ray2.position();

	T a = dot(ray1.direction(), ray1.direction());
	T b = dot(ray1.direction(), ray2.direction());
	T c = dot(ray2.direction(), ray2.direction());
	T d = dot(ray1.direction(), ray1.position() - ray2.position());
	T e = dot(ray2.direction(), ray1.position() - ray2.position());

	s = (b*e - c*d)/(a*c - b*b);
	t = (a*e - b*d)/(a*c - b*b);

	return abs(dot(uv/length(uv), a_b));
}

/*
 * Computes the signed distance between a point and a plane.
 */
template <typename T, int N>
T distance(const plane<T,N>& plane, const point<T,N>& x) {

	// project vector from plane point to x on plane normal
	return dot(x - plane.position(),plane.normal())/length(plane.normal());
}

} // namespace util

#endif // UTIL_GEOMETRY_H__

