#ifndef UTIL_POINT_H__
#define UTIL_POINT_H__

#include <sstream>
#include <iostream>

namespace util {

template <typename T>
struct point {

	point() {};

	template <typename S>
	point(const S& x_, const S& y_) :
		x(x_),
		y(y_) {}

	template <typename S>
	point(const point<S>& other) :
		x(other.x),
		y(other.y) {}

	T x;
	T y;

	template <typename S>
	point<T>& operator+=(const point<S>& other) {

		x += other.x;
		y += other.y;

		return *this;
	}

	template <typename S>
	point<T>& operator-=(const point<S>& other) {

		x -= other.x;
		y -= other.y;

		return *this;
	}

	point<T> operator-() const {

		return point<T>(-x, -y);
	}

	template <typename S>
	point<T>& operator*=(const S& s) {

		x *= s;
		y *= s;

		return *this;
	}

	template <typename S>
	point<T>& operator/=(const S& s) {

		x /= s;
		y /= s;

		return *this;
	}

	template <typename S>
	point<T>& operator*=(const point<S>& p) {

		x *= p.x;
		y *= p.y;

		return *this;
	}

	template <typename S>
	point<T>& operator/=(const point<S>& p) {

		x /= p.x;
		y /= p.y;

		return *this;
	}

	bool operator==(const point<T>& other) const {

		return x == other.x && y == other.y;
	}

	bool operator!=(const point<T>& other) const {

		return !(*this == other);
	}
};

} // namespace util

template <typename T, typename S>
util::point<T> operator+(const util::point<T>& p, const util::point<S>& o) {

	util::point<T> result(p);

	return result += o;
}

template <typename T, typename S>
util::point<T> operator-(const util::point<T>& p, const util::point<S>& o) {

	util::point<T> result(p);

	return result -= o;
}

template <typename T, typename S>
util::point<T> operator*(const util::point<T>& p, const S& s) {

	util::point<T> result(p);

	return result *= s;
}

template <typename T>
util::point<T> operator*(const T& s, const util::point<T>& p) {

	util::point<T> result(p);

	return result *= s;
}

template <typename T, typename S>
util::point<T> operator/(const util::point<T>& p, const S& s) {

	util::point<T> result(p);

	return result /= s;
}

namespace util {

template <typename T>
std::ostream& operator<<(std::ostream& os, const util::point<T>& point) {

	os << "(" << point.x << ", " << point.y << ")";

	return os;
}

} // namespace util

#endif // UTIL_POINT_H__

