#ifndef BOX_H__
#define BOX_H__

#include <util/point3.hpp>
#include <util/point.hpp>
#include <util/rect.hpp>

using util::point3;
using util::point;
using util::rect;

/**
 * Box - a class that represents a 3D rectangular geometry.
 * 
 * This class is designed with packing in mind, and is therefore closed-open. In other words,
 * it contains its minimum bound (returned by location), but does not contain its upper bound,
 * which can be calcuated as location() + size(). It will contain any point that is dimension-wise
 * greater-than-or-equal-to location() and less-than location() + size().
 * 
 * @param T a numerical representation class, which defaults to unsigned int.
 */

template<class T = unsigned int>
class Box : public pipeline::Data
{
public:
	Box() : _location(0, 0, 0), _size(0, 0, 0) {}
	
	Box(const point3<T>& location, const point3<T>& size) :
		_location(location), _size(size) {}
	
	/**
	 * Create a Box that contains a rect for a given z-interval.
	 * @param rect the rect to contain
	 * @param zMin the minimum z for the z-interval
	 * @param depth the depth of the z-interval
	 */
	Box(const rect<T>& rect, T zMin, T depth) :
		_location(rect.minX, rect.minY, zMin), 
	    _size(rect.width(), rect.height(), depth)
			{}

	/**
	 * @returns the point3 representing the minimum bound of this Box.
	 */
	const point3<T> location() const
	{
		return _location;
	}
	
	/**
	 * @returns the point3 representing the size of this box.
	 * 
	 */
	const point3<T> size() const
	{
		return _size;
	}
	
	operator rect<T>() const
	{
		return util::rect<T>(_location.x, _location.y,
							 _location.x + _size.x, _location.y + _size.y);
	}
	
	template<typename S>
	bool contains(const point<S>& loc) const
	{
		return loc.x >= _location.x && loc.y >= _location.y &&
			loc.x < _location.x + _size.x && loc.y < _location.y + _size.y;
	}
	
	template<typename S>
	bool contains(const point3<S>& loc) const
	{
		return loc >= _location && loc < _location + _size;
	}
	
	template <typename S>
	bool contains(const rect<S>& rect) const
	{
		return contains(point<S>(rect.minX, rect.minY)) &&
			contains(point<S>(rect.maxX - 1 , rect.maxY - 1));
	}
	
	template <typename S>
	bool contains(const Box<S>& box) const
	{
		return location() <= box.location() &&
			(location() + size() >= box.location() + box.size());
	}
	
protected:
	point3<T> _location;
	point3<T> _size;
};


template <typename T>
std::ostream& operator<<(std::ostream& os, const Box<T>& box)
{
	util::point3<T> loc = box.location();
	util::point3<T> size = box.size();
	
	os << "[" << loc.x << ", " << loc.y << ", " << loc.z << "; "
	   << size.x + loc.x << ", " << size.y + loc.y << ", " << size.z + loc.z << "]";

	return os;
}

#endif //BOX_H__
