#ifndef UTIL_POINT3_H__
#define UTIL_POINT3_H__

#include <sstream>

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
	bool operator>=(const point3<S>& other) const {

		return !(*this < other);
	}

	template<typename S>
	bool operator<=(const point3<S>& other) const {

		return (*this < other) || (*this == other);
	}

	template<typename S>
	bool operator>(const point3<S>& other) const
	{
		return !(*this <= other);
	}
	
	template<typename S>
	bool operator<(const point3<S>& other) const {

		if (x != other.x)
			return (x < other.x);

		if (y != other.y)
			return (y < other.y);

		return (z < other.z);
	}

	bool operator==(const point3<T>& other) const {

		return x == other.x && y == other.y && z == other.z;
	}

	bool operator!=(const point3<T>& other) const {

		return !(*this == other);
	}

	point3<T> min(point3<T> pt) const {

		return point3<T>(std::min(x, pt.x), std::min(y, pt.y), std::min(z, pt.z));
	}

	point3<T> max(point3<T> pt) const {

		return point3<T>(std::max(x, pt.x), std::max(y, pt.y), std::max(z, pt.z));
	}
};

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

template <typename T>
util::point3<T> operator*(const util::point3<T>& p1, const util::point3<T>& p2) {

	util::point3<T> result(p1);

	return result *= p2;
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

