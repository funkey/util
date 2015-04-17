#ifndef UTIL_PLANE_H__
#define UTIL_PLANE_H__

namespace util {

template <typename T, int N>
class plane {

public:

	plane() {}

	plane(
			const util::point<T,N>& position,
			const util::point<T,N>& normal) :
		_position(position),
		_normal(normal) {}

	const util::point<T,N>& position() const { return _position; }
	const util::point<T,N>& normal()   const { return _normal; }
	util::point<T,N>& position() { return _position; }
	util::point<T,N>& normal()   { return _normal; }

private:

	util::point<T,N> _position;
	util::point<T,N> _normal;
};

template <typename T, int N>
std::ostream& operator<<(std::ostream& os, const util::plane<T,N>& plane) {

	os << "{" << plane.position() << ": " << plane.normal() << "}";
	return os;
}

} // namespace util

#endif // UTIL_PLANE_H__

