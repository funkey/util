#ifndef UTIL_BOX_H__
#define UTIL_BOX_H__

#include <sstream>
#include <iostream>
#include <type_traits>

#include "point.hpp"

namespace util {

template <typename T>
struct box {

	/**
	 * Default constructor.
	 */
	box() : minX(0), minY(0), minZ(0), maxX(0), maxY(0), maxZ(0) {};

	template <typename S>
	box(const box<S>& other) :
		minX(other.minX),
		minY(other.minY),
		minZ(other.minZ),
		maxX(other.maxX),
		maxY(other.maxY),
		maxZ(other.maxZ) {};

	box(const T& minX_, const T& minY_, const T& minZ_, const T& maxX_, const T& maxY_, const T& maxZ_) :
		minX(minX_),
		minY(minY_),
		minZ(minZ_),
		maxX(maxX_),
		maxY(maxY_),
		maxZ(maxZ_) {};

	T minX;
	T minY;
	T minZ;
	T maxX;
	T maxY;
	T maxZ;

	T width() const {

		return maxX - minX;
	}

	T height() const {

		return maxY - minY;
	}

	T depth() const {

		return maxZ - minZ;
	}

	bool valid() { return volume() > 0; }

	template <typename S>
	bool intersects(const box<S>& other) const {

		// empty boxes do not intersect
		if (volume() <= 0 || other.volume() <= 0)
			return false;

		// two non-intersecting boxes are separated by a plane parallel to
		// either x, y, or z

		// separated by x-plane
		if (maxX <= other.minX || minX >= other.maxX)
			return false;

		// separated by y-plane
		if (maxY <= other.minY || minY >= other.maxY)
			return false;

		// separated by z-plane
		if (maxZ <= other.minZ || minZ >= other.maxZ)
			return false;

		return true;
	}

	template <typename S>
	box<T> intersection(const box<S>& other) const {

		box<T> result(0, 0, 0, 0, 0, 0);

		if (!intersects(other))
			return result;

		result.minX = std::max(minX, other.minX);
		result.minY = std::max(minY, other.minY);
		result.minZ = std::max(minZ, other.minZ);
		result.maxX = std::min(maxX, other.maxX);
		result.maxY = std::min(maxY, other.maxY);
		result.maxZ = std::min(maxZ, other.maxZ);

		return result;
	}

	template <typename S>
	bool contains(S x, S y, S z) const {

		if (maxX <= x || minX > x || maxY <= y || minY > y || maxZ <= z || minZ > z)
			return false;

		return true;
	}

	template <typename PointType>
	bool contains(const PointType& point) const {

		return contains(point[0], point[1], point[2]);
	}

	/**
	 * Extend this box, such that it fits the given point.
	 */
	template <typename PointType>
	void fit(const PointType& point) {

		if (isZero()) {

			minX = point[0];
			maxX = point[0];
			minY = point[1];
			maxY = point[1];
			minZ = point[2];
			maxZ = point[2];
			return;
		}

		minX = std::min(point[0], minX);
		minY = std::min(point[1], minY);
		minZ = std::min(point[2], minZ);
		maxX = std::max(point[0], maxX);
		maxY = std::max(point[1], maxY);
		maxZ = std::max(point[2], maxZ);
	}

	/**
	 * Extend this box, such that it fits the given box.
	 */
	void fit(const box<T>& box) {

		if (isZero()) {

			*this = box;
			return;
		}

		minX = std::min(box.minX, minX);
		minY = std::min(box.minY, minY);
		minZ = std::min(box.minZ, minZ);
		maxX = std::max(box.maxX, maxX);
		maxY = std::max(box.maxY, maxY);
		maxZ = std::max(box.maxZ, maxZ);
	}

	/**
	 * Extend this box, such that it fits the given box.
	 */
	box<T>& operator+=(const box<T>& other) {

		this->fit(other);
		return *this;
	}

	/**
	 * Add two bounding boxes, i.e., create one that fits both.
	 */
	box<T> operator+(const box<T>& other) const {

		box<T> b(*this);
		return (b += other);
	}

	T volume() const {

		return width()*height()*depth();
	}

	bool isZero() const {

		return minX == 0 && minY == 0 && maxX == 0 && maxY == 0 && minZ == 0 && maxZ == 0;
	}

	template <typename PointType>
	box<T>& operator+=(const PointType& other) {

		minX += other[0];
		maxX += other[0];
		minY += other[1];
		maxY += other[1];
		minZ += other[2];
		maxZ += other[2];

		return *this;
	}

	template <typename PointType>
	box<T>& operator-=(const PointType& other) {

		minX -= other[0];
		maxX -= other[0];
		minY -= other[1];
		maxY -= other[1];
		minZ -= other[2];
		maxZ -= other[2];

		return *this;
	}

	template <typename S>
	box<T>& operator*=(const S& s) {

		mul(s, typename std::is_arithmetic<S>::type());

		return *this;
	}

	template <typename S>
	box<T>& operator/=(const S& s) {

		minX /= s;
		maxX /= s;
		minY /= s;
		maxY /= s;
		minZ /= s;
		maxZ /= s;

		return *this;
	}

	template <typename S>
	bool operator==(const box<S>& other) const {

		return minX == other.minX &&
		       maxX == other.maxX &&
		       minY == other.minY &&
		       maxY == other.maxY &&
		       minZ == other.minZ &&
		       maxZ == other.maxZ;
	}

	template <typename S>
	bool operator!=(const box<S>& other) const {

		return !(*this == other);
	}

private:

	template <typename S>
	void mul(S s, std::true_type is_arithmetic) {

		minX *= s;
		maxX *= s;
		minY *= s;
		maxY *= s;
		minZ *= s;
		maxZ *= s;
	}

	template <typename S>
	void mul(S s, std::false_type is_arithmetic) {

		minX *= s[0];
		maxX *= s[0];
		minY *= s[1];
		maxY *= s[1];
		minZ *= s[2];
		maxZ *= s[2];
	}
};

} // namespace util

template <typename T, typename PointType>
util::box<T> operator+(const util::box<T>& p, const PointType& o) {

	util::box<T> result(p);

	return result += o;
}

template <typename T, typename PointType>
util::box<T> operator-(const util::box<T>& p, const PointType& o) {

	util::box<T> result(p);

	return result -= o;
}

template <typename T, typename S>
util::box<T> operator*(const util::box<T>& p, const S& s) {

	util::box<T> result(p);

	return result *= s;
}

template <typename T, typename S>
util::box<S> operator*(const S& s, const util::box<T>& p) {

	util::box<S> result(p);

	return result *= s;
}

template <typename T, typename S>
util::box<T> operator/(const util::box<T>& p, const S& s) {

	util::box<T> result(p);

	return result /= s;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const util::box<T>& box) {

	os <<  "[(" << box.minX << ", " << box.minY << ", " << box.minZ << ")"
	   << ", (" << box.maxX << ", " << box.maxY << ", " << box.maxZ << ")]";

	return os;
}

#endif // UTIL_BOX_H__

