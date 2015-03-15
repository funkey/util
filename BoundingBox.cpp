#include "BoundingBox.h"

std::ostream& operator<<(std::ostream& os, const BoundingBox& boundingBox) {

	os
			<< "[("
			<< boundingBox.getMinX() << ", " 
			<< boundingBox.getMinY() << ", " 
			<< boundingBox.getMinZ() << "), (" 
			<< boundingBox.getMaxX() << ", " 
			<< boundingBox.getMaxY() << ", " 
			<< boundingBox.getMaxZ() << ")]";

	return os;
}
