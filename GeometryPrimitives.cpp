#include "GeometryPrimitives.h"

std::ostream& operator<<(std::ostream& os, const Point3D& p) {
	os << "[ x = " << p.x << "; y = " << p.y << " ; z = " << p.z << " ]";
	return os;
}

std::vector<Point3D> Triangle::points() const {
	return std::vector<Point3D>{a, b, c};
}

std::ostream& operator<<(std::ostream& os, const Triangle& triangle) {
	os << triangle.a << triangle.b << triangle.c;
	return os;
}
