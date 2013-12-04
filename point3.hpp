#ifndef UTIL_POINT3_H__
#define UTIL_POINT3_H__

#include <sstream>
#include <iostream>
#include <boost/functional/hash.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <util/point.hpp>

namespace util {

template <typename T>
struct point3 {

	point3() {};

	template <typename S>
	point3(const S& x_, const S& y_, const S& z_) :
		x(x_),
		y(y_),
		z(z_) {}

	template <typename S>
	point3(const point3<S>& other) :
		x(other.x),
		y(other.y),
		z(other.z){}

	T x;
	T y;
	T z;

	template <typename S>
	point3<T>& operator+=(const point3<S>& other) {

		x += other.x;
		y += other.y;
		z += other.z;

		return *this;
	}

	template <typename S>
	point3<T>& operator-=(const point3<S>& other) {

		x -= other.x;
		y -= other.y;
		z -= other.z;

		return *this;
	}

	point3<T> operator-() const {

		return point3<T>(-x, -y, -z);
	}

	template <typename S>
	point3<T>& operator*=(const S& s) {

		x *= s;
		y *= s;
		z *= s;

		return *this;
	}

	template <typename S>
	point3<T>& operator/=(const S& s) {

		x /= s;
		y /= s;
		z /= s;

		return *this;
	}

	template <typename S>
	point3<T>& operator*=(const point3<S>& p) {

		x *= p.x;
		y *= p.y;
		z *= p.z;

		return *this;
	}

	template <typename S>
	point3<T>& operator/=(const point3<S>& p) {

		x /= p.x;
		y /= p.y;
		z /= p.z;

		return *this;
	}
	
	template<typename S>
	bool operator>=(const point3<S>& other) const
	{
		return x >= other.x && y >= other.y && z >= other.z;
	}
	
	template<typename S>
	bool operator<=(const point3<S>& other) const
	{
		return x <= other.x && y <= other.y && z <= other.z;
	}
	
	template<typename S>
	bool operator>(const point3<S>& other) const
	{
		return x > other.x && y > other.y && z > other.z;
	}
	
	template<typename S>
	bool operator<(const point3<S>& other) const
	{
		return x < other.x && y < other.y && z < other.z;
	}

	bool operator==(const point3<T>& other) const {

		return x == other.x && y == other.y && z == other.z;
	}

	bool operator!=(const point3<T>& other) const {

		return !(*this == other);
	}
	
	
	operator util::point<T>()
	{
		return util::point<T>(x, y);
	}

};

template <class T>
std::size_t hash_value(point3<T> const& point)
{
	std::size_t seed = 0;
	boost::hash_combine(seed, boost::hash_value(point.x));
	boost::hash_combine(seed, boost::hash_value(point.y));
	boost::hash_combine(seed, boost::hash_value(point.z));
	return seed;
}

template<class T>
boost::shared_ptr<point3<T> > ptrTo(T x, T y, T z)
{
	boost::shared_ptr<point3<T> > pt = boost::make_shared<point3<T> >(x, y, z);
	return pt;
}

} // namespace util

template <typename T, typename S>
util::point3<T> operator+(const util::point3<T>& p, const util::point3<S>& o) {

	util::point3<T> result(p);

	return result += o;
}

template <typename T, typename S>
util::point3<T> operator-(const util::point3<T>& p, const util::point3<S>& o) {

	util::point3<T> result(p);

	return result -= o;
}

template <typename T, typename S>
util::point3<T> operator*(const util::point3<T>& p, const S& s) {

	util::point3<T> result(p);

	return result *= s;
}

template <typename T>
util::point3<T> operator*(const T& s, const util::point3<T>& p) {

	util::point3<T> result(p);

	return result *= s;
}

template <typename T, typename S>
util::point3<T> operator/(const util::point3<T>& p, const S& s) {

	util::point3<T> result(p);

	return result /= s;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const util::point3<T>& point) {

	os << "(" << point.x << ", " << point.y << ", " << point.z << ")";

	return os;
}


#endif // UTIL_POINT3_H__

