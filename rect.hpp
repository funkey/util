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

	template <typename S>
	rect(const rect<S>& other) :
		minX(other.minX),
		minY(other.minY),
		maxX(other.maxX),
		maxY(other.maxY) {};

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

	point<T> upperLeft() const {

		return point<T>(minX, minY);
	}

	point<T> lowerRight() const {

		return point<T>(maxX, maxY);
	}

	point<T> center() const {

		return point<T>((minX + maxX)/2, (minY + maxY)/2);
	}

	bool intersects(const rect<T>& other) const {

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

	rect<T> intersection(const rect<T>& other) const {

		rect<T> result(0, 0, 0, 0);

		if (!intersects(other))
			return result;

		result.minX = std::max(minX, other.minX);
		result.minY = std::max(minY, other.minY);
		result.maxX = std::min(maxX, other.maxX);
		result.maxY = std::min(maxY, other.maxY);

		return result;
	}

	bool contains(const point<T>& point) const {

		return minX <= point.x && minY <= point.y && maxX > point.x && maxY > point.y;
	}

	T area() const {

		return width()*height();
	}

	template <typename S>
	rect<T>& operator+=(const point<S>& other) {

		minX += other.x;
		maxX += other.x;
		minY += other.y;
		maxY += other.y;

		return *this;
	}

	template <typename S>
	rect<T>& operator-=(const point<S>& other) {

		minX -= other.x;
		maxX -= other.x;
		minY -= other.y;
		maxY -= other.y;

		return *this;
	}

	template <typename S>
	rect<T>& operator*=(const S& s) {

		minX *= s;
		maxX *= s;
		minY *= s;
		maxY *= s;

		return *this;
	}

	template <typename S>
	rect<T>& operator/=(const S& s) {

		minX /= s;
		maxX /= s;
		minY /= s;
		maxY /= s;

		return *this;
	}

	template <typename S>
	rect<T>& operator*=(const point<S>& p) {

		minX *= p.x;
		maxX *= p.x;
		minY *= p.y;
		maxY *= p.y;

		return *this;
	}

	template <typename S>
	rect<T>& operator/=(const point<S>& p) {

		minX /= p.x;
		maxX /= p.x;
		minY /= p.y;
		maxY /= p.y;

		return *this;
	}

	template <typename S>
	bool operator==(const rect<S>& other) const {

		return minX == other.minX &&
		       minY == other.minY &&
		       maxX == other.maxX &&
		       maxY == other.maxY;
	}

	template <typename S>
	bool operator!=(const rect<S>& other) const {

		return !(*this == other);
	}
};

} // namespace util

template <typename T, typename S>
util::rect<T> operator+(const util::rect<T>& p, const util::point<S>& o) {

	util::rect<T> result(p);

	return result += o;
}

template <typename T, typename S>
util::rect<T> operator-(const util::rect<T>& p, const util::point<S>& o) {

	util::rect<T> result(p);

	return result -= o;
}

template <typename T, typename S>
util::rect<T> operator*(const util::rect<T>& p, const S& s) {

	util::rect<T> result(p);

	return result *= s;
}

template <typename T, typename S>
util::rect<T> operator*(const S& s, const util::rect<T>& p) {

	util::rect<T> result(p);

	return result *= s;
}

template <typename T, typename S>
util::rect<T> operator/(const util::rect<T>& p, const S& s) {

	util::rect<T> result(p);

	return result /= s;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const util::rect<T>& rect) {

	os << "[" << rect.minX << ", " << rect.minY
	   << ", " << rect.maxX << ", " << rect.maxY << "]";

	return os;
}

#endif // UTIL_RECT_H__

