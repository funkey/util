#ifndef UTIL_POINT_H__
#define UTIL_POINT_H__

#include <array>
#include <sstream>
#include <iostream>

namespace util {

// forward declaration
template <typename T, int N>
class point;

template <typename Derived, typename T, int N>
class point_base {

public:

	point_base() {

		_a.fill(0);
	}

	template <typename D, typename S>
	point_base(const point_base<D, S, N>& other) {

		for (int i = 0; i < N; i++)
			_a[i] = other.data()[i];
	}

	const T& operator[](int i) const { return _a[i]; }
	T& operator[](int i) { return _a[i]; }

	bool isZero() const {

		for (int i = 0; i < N; i++)
			if (_a[i] != 0)
				return false;
		return true;
	}

	template <typename D, typename S>
	Derived& operator+=(const point_base<D, S, N>& other) {

		for (int i = 0; i < N; i++)
			_a[i] += other.data()[i];

		return static_cast<Derived&>(*this);
	}

	template <typename D, typename S>
	Derived& operator-=(const point_base<D, S, N>& other) {

		for (int i = 0; i < N; i++)
			_a[i] -= other.data()[i];

		return static_cast<Derived&>(*this);
	}

	Derived operator-() const {

		Derived p;
		for (int i = 0; i < N; i++)
			p._a[i] = -data()[i];
		return p;
	}

	template <typename S>
	Derived& operator*=(const S& s) {

		mul(s, typename std::is_arithmetic<S>::type());

		return static_cast<Derived&>(*this);
	}

	template <typename S>
	Derived& operator/=(const S& s) {

		div(s, typename std::is_arithmetic<S>::type());

		return static_cast<Derived&>(*this);
	}

	template <typename D, typename S>
	Derived& operator*=(const point_base<D, S, N>& other) {

		for (int i = 0; i < N; i++)
			_a[i] *= other.data()[i];

		return static_cast<Derived&>(*this);
	}

	template <typename D, typename S>
	Derived& operator/=(const point_base<D, S, N>& other) {

		for (int i = 0; i < N; i++)
			_a[i] /= other.data()[i];

		return static_cast<Derived&>(*this);
	}

	template <typename D, typename S>
	bool operator==(const point_base<D, S, N>& other) const {

		for (int i = 0; i < N; i++)
			if (_a[i] != other.data()[i])
				return false;
		return true;
	}

	template <typename D, typename S>
	bool operator!=(const point_base<D, S, N>& other) const {

		return !(*this == other);
	}

	template <int M>
	explicit operator point<T,M>() const {

		point<T,M> projected;
		for (int i = 0; i < std::min(N, M); i++)
			projected[i] = _a[i];

		return projected;
	}

	/**
	 * Project to a different dimension. If the target dimension is smaller, the 
	 * last components of the point are simply discarded. If the target 
	 * dimension is larger, new zero-initialized components are added.
	 */
	template <int M>
	point<T,M> project() const {

		return static_cast<util::point<T,M>>(*this);
	}

	inline const std::array<T, N>& data() const { return _a; }
	inline std::array<T, N>& data() { return _a; }

protected:

	std::array<T, N> _a;

private:

	template <typename S>
	void mul(S s, std::true_type /*is_arithmetic*/) {

		for (int i = 0; i < N; i++)
			_a[i] *= s;
	}

	template <typename S>
	void mul(S s, std::false_type /*is_arithmetic*/) {

		for (int i = 0; i < N; i++)
			_a[i] *= s[i];
	}

	template <typename S>
	void div(S s, std::true_type /*is_arithmetic*/) {

		for (int i = 0; i < N; i++)
			_a[i] /= s;
	}

	template <typename S>
	void div(S s, std::false_type /*is_arithmetic*/) {

		for (int i = 0; i < N; i++)
			_a[i] /= s[i];
	}
};

template <typename T, int N>
class point : public point_base<point<T, N>, T, N> {};

template <typename T>
class point<T, 2> : public point_base<point<T, 2>, T, 2> {

	typedef point_base<point<T, 2>, T, 2> base_type;

public:

	point() {};

	point(T x_, T y_) {

		x() = x_;
		y() = y_;
	}

	template <typename S>
	point(const point<S, 2>& other) :
		base_type(other) {}

	using base_type::data;

	inline const T& x() const { return data()[0]; }
	inline const T& y() const { return data()[1]; }
	inline T& x() { return data()[0]; }
	inline T& y() { return data()[1]; }
};

template <typename T>
class point<T, 3> : public point_base<point<T, 3>, T, 3> {

	typedef point_base<point<T, 3>, T, 3> base_type;

public:

	point() {};

	point(T x_, T y_, T z_) {

		x() = x_;
		y() = y_;
		z() = z_;
	}

	template <typename S>
	point(const point<S, 3>& other) :
		base_type(other) {}

	using base_type::data;

	inline const T& x() const { return data()[0]; }
	inline const T& y() const { return data()[1]; }
	inline const T& z() const { return data()[2]; }
	inline T& x() { return data()[0]; }
	inline T& y() { return data()[1]; }
	inline T& z() { return data()[2]; }
};

} // namespace util

template <typename T, typename S, int N>
util::point<T, N> operator+(const util::point<T, N>& p, const util::point<S, N>& o) {

	util::point<T, N> result(p);

	return result += o;
}

template <typename T, typename S, int N>
util::point<T, N> operator-(const util::point<T, N>& p, const util::point<S, N>& o) {

	util::point<T, N> result(p);

	return result -= o;
}

template <typename T, typename S, int N>
util::point<T, N> operator*(const util::point<T, N>& p, const S& s) {

	util::point<T, N> result(p);

	return result *= s;
}

template <typename T, int N>
util::point<T, N> operator*(const T& s, const util::point<T, N>& p) {

	util::point<T, N> result(p);

	return result *= s;
}

template <typename T, typename S, int N>
util::point<T, N> operator/(const util::point<T, N>& p, const S& s) {

	util::point<T, N> result(p);

	return result /= s;
}

namespace util {

template <typename T, int N>
std::ostream& operator<<(std::ostream& os, const util::point<T, N>& point) {

	os << "(";
	for (int i = 0; i < N; i++)
		os << point[i] << (i == N-1 ? "" : ", ");
	os << ")";

	return os;
}

} // namespace util

#endif // UTIL_POINT_H__

