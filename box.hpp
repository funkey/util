#ifndef UTIL_BOX_H__
#define UTIL_BOX_H__

#include <sstream>
#include <iostream>
#include <type_traits>

#include "point.hpp"

namespace util {

// forward declaration
template <typename T, int N>
class box;

template <typename Derived, typename T, int N>
class box_base {

public:

	/**
	 * Default constructor.
	 */
	box_base() {}

	template <typename S>
	box_base(const point<S,N>& min, const point<S,N>& max) :
		_min(min),
		_max(max) {}

	template <typename D,typename S>
	box_base(const box_base<D,S,N>& other) :
		_min(other.min()),
		_max(other.max()) {}

	template <typename S>
	operator box<S,N>() const { return box<S,N>(_min, _max); }

	const point<T,N>& min() const { return _min; }
	point<T,N>& min() { return _min; }
	const point<T,N>& max() const { return _max; }
	point<T,N>& max() { return _max; }

	bool valid() { return isZero(); }

	point<T,N> center() const { return (_min + _max)/2.0; }

	template <typename D, typename S>
	bool intersects(const box_base<D,S,N>& other) const {

		// empty boxes do not intersect
		if (isZero() || other.isZero())
			return false;

		// two non-intersecting boxes are separated by a plane parallel to
		// either dimension

		for (int i = 0; i < N; i++)
			if (max()[i] <= other.min()[i] || min()[i] >= other.max()[i])
				return false;

		return true;
	}

	template <typename D, typename S>
	Derived intersection(const box_base<D,S,N>& other) const {

		Derived result;

		if (!intersects(other))
			return result;

		for (int i = 0; i < N; i++) {

			result.min()[i] = std::max(min()[i], other.min()[i]);
			result.max()[i] = std::min(max()[i], other.max()[i]);
		}

		return result;
	}

	template <typename S, int M>
	bool contains(const point<S,M>& point) const {

		for (int i = 0; i < std::min(N, M); i++)
			if (max()[i] <= point[i] || min()[i] > point[i])
				return false;

		return true;
	}

	/**
	 * Extend this box, such that it fits the given point.
	 */
	template <typename S>
	void fit(const point<S,N>& point) {

		if (isZero()) {

			for (int i = 0; i < N; i++) {

				min()[i] = point[i];
				max()[i] = point[i];
			}

			return;
		}

		for (int i = 0; i < N; i++) {

			min()[i] = std::min((T)point[i], min()[i]);
			max()[i] = std::max((T)point[i], max()[i]);
		}
	}

	/**
	 * Extend the x and y dimensions of the box, such that it fits the given 
	 * box.
	 */
	template <typename D, typename S, int M>
	void fit(const box_base<D,S,M>& other) {

		if (other.isZero())
			return;

		if (isZero()) {

			for (int i = 0; i < std::min(N, M); i++) {

				min()[i] = other.min()[i];
				max()[i] = other.max()[i];
			}

			return;
		}

		for (int i = 0; i < std::min(N, M); i++) {

			min()[i] = std::min(other.min()[i], min()[i]);
			max()[i] = std::max(other.max()[i], max()[i]);
		}
	}

	/**
	 * Extend this box, such that it fits the given box.
	 */
	template <typename D, typename S, int M>
	Derived& operator+=(const box_base<D,S,M>& other) {

		this->fit(other);

		return static_cast<Derived&>(*this);
	}

	/**
	 * Add two boxes, i.e., create one that fits both.
	 */
	template <typename D, typename S, int M>
	Derived operator+(const box_base<D,S,M>& other) const {

		Derived b(*this);
		return (b += other);
	}

	bool isZero() const {

		for (int i = 0; i < N; i++)
			if (min()[i] != 0 || max()[i] != 0)
				return false;

		return true;
	}

	template <typename S, int M>
	Derived& operator+=(const point<S,M>& p) {

		for (int i = 0; i < std::min(N, M); i++) {

			min()[i] += p[i];
			max()[i] += p[i];
		}

		return static_cast<Derived&>(*this);
	}

	template <typename S, int M>
	Derived& operator-=(const point<S,M>& p) {

		for (int i = 0; i < std::min(N, M); i++) {

			min()[i] -= p[i];
			max()[i] -= p[i];
		}

		return static_cast<Derived&>(*this);
	}

	template <typename S>
	Derived& operator*=(const S& s) {

		min() *= s;
		max() *= s;

		return static_cast<Derived&>(*this);
	}

	template <typename S>
	Derived& operator/=(const S& s) {

		min() /= s;
		max() /= s;

		return static_cast<Derived&>(*this);
	}

	template <typename D, typename S>
	bool operator==(const box_base<D,S,N>& other) const {

		return (min() == other.min() && max() == other.max());
	}

	template <typename D, typename S>
	bool operator!=(const box_base<D,S,N>& other) const {

		return !(*this == other);
	}

	/**
	 * Project to a different dimension. If the target dimension is smaller, the 
	 * last components of the point are simply discarded. If the target 
	 * dimension is larger, new zero-initialized components are added.
	 */
	template <int M>
	box<T,M> project() const {

		return box_base<box<T,M>, T, M>(
				_min.template project<M>(),
				_max.template project<M>());
	}

private:

	point<T,N> _min;
	point<T,N> _max;
};

template <typename T, int N>
class box : public box_base<box<T,N>, T, N> {

	typedef box_base<box<T,N>, T, N> base_type;

public:

	template <typename S>
	box(const point<S,N>& min, const point<S,N>& max) :
		base_type(
				min,
				max) {}
};

template <typename T>
class box<T,2> : public box_base<box<T,2>, T, 2> {

	typedef box_base<box<T,2>, T, 2> base_type;

public:

	box() : base_type() {}

	box(T minX, T minY, T maxX, T maxY) :
		base_type(
				point<T,2>(minX, minY),
				point<T,2>(maxX, maxY)) {}

	template <typename S>
	box(const point<S,2>& min, const point<S,2>& max) :
		base_type(
				min,
				max) {}

	using base_type::min;
	using base_type::max;

	T width() const {

		return max().x() - min().x();
	}

	T height() const {

		return max().y() - min().y();
	}

	T area() const {

		return width()*height();
	}

	using base_type::contains;

	template <typename S>
	bool contains(S x, S y) const {

		return contains(point<S,2>(x, y));
	}
};

template <typename T>
class box<T, 3> : public box_base<box<T, 3>, T, 3> {

	typedef box_base<box<T,3>, T, 3> base_type;

public:

	box() : base_type() {}

	box(T minX, T minY, T minZ, T maxX, T maxY, T maxZ) :
		base_type(
				point<T,3>(minX, minY, minZ),
				point<T,3>(maxX, maxY, maxZ)) {}

	template <typename S>
	box(const point<S,3>& min, const point<S,3>& max) :
		base_type(
				min,
				max) {}

	using base_type::min;
	using base_type::max;

	T width() const {

		return max().x() - min().x();
	}

	T height() const {

		return max().y() - min().y();
	}

	T depth() const {

		return max().z() - min().z();
	}

	T volume() const {

		return width()*height()*depth();
	}

	using base_type::contains;

	template <typename S>
	bool contains(S x, S y, S z) const {

		return contains(point<S,3>(x, y, z));
	}
};

} // namespace util

template <typename T, int N, typename PointType>
util::box<T,N> operator+(const util::box<T,N>& p, const PointType& o) {

	util::box<T,N> result(p);

	return result += o;
}

template <typename T, int N, typename PointType>
util::box<T,N> operator-(const util::box<T,N>& p, const PointType& o) {

	util::box<T,N> result(p);

	return result -= o;
}

template <typename T, int N, typename S>
util::box<T,N> operator*(const util::box<T,N>& p, const S& s) {

	util::box<T,N> result(p);

	return result *= s;
}

template <typename S, typename T, int N>
util::box<S,N> operator*(const S& s, const util::box<T,N>& p) {

	util::box<S,N> result(p);

	return result *= s;
}

template <typename T, int N, typename S>
util::box<T,N> operator/(const util::box<T,N>& p, const S& s) {

	util::box<T,N> result(p);

	return result /= s;
}

namespace util {

template <typename T, int N>
std::ostream& operator<<(std::ostream& os, const box<T,N>& box) {

	os <<  "[" << box.min() << ", " << box.max() << "]";

	return os;
}

} // namespace util

#endif // UTIL_BOX_H__

