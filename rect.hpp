#ifndef UTIL_RECT_H__
#define UTIL_RECT_H__

#include <sstream>
#include <iostream>

#include "point.hpp"

namespace util {

template <typename T>
struct rect {

	/**
	 * Default constructor.
	 */
	rect() {};

	rect(const T& minX_, const T& minY_, const T& maxX_, const T& maxY_) :
		minX(minX_),
		minY(minY_),
		maxX(maxX_),
		maxY(maxY_) {};

	T minX;
	T minY;
	T maxX;
	T maxY;

	T width() const {

		return maxX - minX;
	}

	T height() const {

		return maxY - minY;
	}

	bool intersects(const rect<T>& other) {

		// two non-intersecting rectanlges are separated by a line parallel to
		// either x or y

		// separated by x-line
		if (maxX < other.minX || minX > other.maxX)
			return false;

		// separated by y-line
		if (maxY < other.minY || minY > other.maxY)
			return false;

		return true;
	}

	bool contains(const point<T>& point) const {

		return minX <= point.x && minY <= point.y && maxX >= point.x && maxY >= point.y;
	}

	rect<T>& operator+=(const point<T>& other) {

		minX += other.x;
		maxX += other.x;
		minY += other.y;
		maxY += other.y;

		return *this;
	}

	rect<T>& operator-=(const point<T>& other) {

		minX -= other.x;
		maxX -= other.x;
		minY -= other.y;
		maxY -= other.y;

		return *this;
	}

	rect<T>& operator*=(const T& s) {

		minX *= s;
		maxX *= s;
		minY *= s;
		maxY *= s;

		return *this;
	}

	rect<T>& operator/=(const T& s) {

		minX /= s;
		maxX /= s;
		minY /= s;
		maxY /= s;

		return *this;
	}

	rect<T>& operator*=(const point<T>& p) {

		minX *= p.x;
		maxX *= p.x;
		minY *= p.y;
		maxY *= p.y;

		return *this;
	}

	rect<T>& operator/=(const point<T>& p) {

		minX /= p.x;
		maxX /= p.x;
		minY /= p.y;
		maxY /= p.y;

		return *this;
	}

	bool operator==(const rect<T>& other) const {

		return minX == other.minX &&
		       minY == other.minY &&
		       maxX == other.maxX &&
		       maxY == other.maxY;
	}

	bool operator!=(const rect<T>& other) const {

		return !(*this == other);
	}
};

template <typename T>
rect<T> operator+(const rect<T>& p, const point<T>& o) {

	rect<T> result(p);

	return result += o;
}

template <typename T>
rect<T> operator-(const rect<T>& p, const point<T>& o) {

	rect<T> result(p);

	return result -= o;
}

template <typename T, typename S>
rect<T> operator*(const rect<T>& p, const S& s) {

	rect<T> result(p);

	return result *= s;
}

template <typename T, typename S>
rect<T> operator/(const rect<T>& p, const S& s) {

	rect<T> result(p);

	return result /= s;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const rect<T>& rect) {

	os << "[" << rect.minX << ", " << rect.minY
	   << ", " << rect.maxX << ", " << rect.maxY << "]";

	return os;
}

} // namespace util

#endif // UTIL_RECT_H__

