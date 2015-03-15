#ifndef IMAGEPROCESSING_BOUNDING_BOX_H__
#define IMAGEPROCESSING_BOUNDING_BOX_H__

#include <algorithm>
#include <ostream>

class BoundingBox {

public:

	/**
	 * Default constructor. Initializes an empty bounding box.
	 */
	BoundingBox() :
		_minX(0),
		_minY(0),
		_minZ(0),
		_maxX(0),
		_maxY(0),
		_maxZ(0) {}

	/**
	 * Create a new bounding box from given coordinates.
	 */
	BoundingBox(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) :
		_minX(minX),
		_minY(minY),
		_minZ(minZ),
		_maxX(maxX),
		_maxY(maxY),
		_maxZ(maxZ) {}

	/**
	 * Set the minimal point of the bounding box.
	 */
	void setMin(float minX, float minY, float minZ) { _minX = minX; _minY = minY; _minZ = minZ; }

	/**
	 * Set the maximal point of the bounding box.
	 */
	void setMax(float maxX, float maxY, float maxZ) { _maxX = maxX; _maxY = maxY; _maxZ = maxZ; }

	/**
	 * Set the minimal point of the bounding box.
	 */
	void setMinX(float minX) { _minX = minX; }
	void setMinY(float minY) { _minY = minY; }
	void setMinZ(float minZ) { _minZ = minZ; }

	/**
	 * Set the maximal point of the bounding box.
	 */
	void setMaxX(float maxX) { _maxX = maxX; }
	void setMaxY(float maxY) { _maxY = maxY; }
	void setMaxZ(float maxZ) { _maxZ = maxZ; }

	/**
	 * Get the minimal point of the bounding box.
	 */
	float getMinX() const { return _minX; }
	float getMinY() const { return _minY; }
	float getMinZ() const { return _minZ; }

	/**
	 * Get the maximal point of the bounding box.
	 */
	float getMaxX() const { return _maxX; }
	float getMaxY() const { return _maxY; }
	float getMaxZ() const { return _maxZ; }

	/**
	 * Get the dimensions of the bounding box.
	 */
	float width()  const { return _maxX - _minX; }
	float height() const { return _maxY - _minY; }
	float depth()  const { return _maxZ - _minZ; }

	/**
	 * Get the volume of the bounding box.
	 */
	float volume() const { return width()*height()*depth(); }

	/**
	 * Add two bounding boxes. The resulting bounding box bounds both boxes.
	 */
	BoundingBox operator+(const BoundingBox& other) const {

		BoundingBox result(*this);
		result += other;

		return result;
	}

	/**
	 * Expand this bounding box to also contain the other one.
	 */
	BoundingBox& operator+=(const BoundingBox& other) {

		if (!valid()) {

			_minX = other._minX;
			_minY = other._minY;
			_minZ = other._minZ;
			_maxX = other._maxX;
			_maxY = other._maxY;
			_maxZ = other._maxZ;

		} else {

			_minX = std::min(_minX, other._minX);
			_minY = std::min(_minY, other._minY);
			_minZ = std::min(_minZ, other._minZ);
			_maxX = std::max(_maxX, other._maxX);
			_maxY = std::max(_maxY, other._maxY);
			_maxZ = std::max(_maxZ, other._maxZ);
		}

		return *this;
	}

	bool operator==(const BoundingBox& other) const {

		return
			_minX == other._minX &&
			_minY == other._minY &&
			_minZ == other._minZ &&
			_maxX == other._maxX &&
			_maxY == other._maxY &&
			_maxZ == other._maxZ;
	}

	/**
	 * Multiply a bounding box with a vector. Each dimension will be scaled with 
	 * the corresponding value in the vector.
	 */
	template <typename PointType>
	BoundingBox operator*(const PointType& point) const {

		BoundingBox result = *this;
		return result *= point;
	}

	/**
	 * Multiply a bounding box with a vector. Each dimension will be scaled with 
	 * the corresponding value in the vector.
	 */
	template <typename PointType>
	BoundingBox& operator*=(const PointType& point) {

		_minX *= point[0];
		_maxX *= point[0];
		_minY *= point[1];
		_maxY *= point[1];
		_minZ *= point[2];
		_maxZ *= point[2];

		return *this;
	}

	bool contains(float x, float y, float z) const {

		if (_maxX <= x || _minX > x || _maxY <= y || _minY > y || _maxZ <= z || _minZ > z)
			return false;

		return true;
	}

	bool valid() const {

		return (width() > 0 || height() > 0 || depth() > 0);
	}

private:

	float _minX;
	float _minY;
	float _minZ;
	float _maxX;
	float _maxY;
	float _maxZ;
};

std::ostream& operator<<(std::ostream& os, const BoundingBox& other);

#endif // IMAGEPROCESSING_BOUNDING_BOX_H__

