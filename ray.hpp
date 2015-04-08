#ifndef UTIL_RAY_H__
#define UTIL_RAY_H__

#include "point.hpp"

namespace util {

template <typename T, int N>
class ray {

public:

	ray() {}

	ray(const point<T,N>& position, const point<T,N>& direction) :
		_position(position),
		_direction(direction) {}

	const point<T,N>& position() const { return _position; }
	point<T,N>& position() { return _position; }

	const point<T,N>& direction() const { return _direction; }
	point<T,N>& direction() { return _direction; }

	point<T,N> at(T t) { return _position + t*_direction; }

private:

	point<T,N> _position;
	point<T,N> _direction;
};

} // namespace util

template <typename T, int N>
std::ostream& operator<<(std::ostream& os, const util::ray<T,N>& ray) {

	os << "{" << ray.position() << "->" << ray.direction() << "}";
	return os;
}

#endif // UTIL_RAY_H__

