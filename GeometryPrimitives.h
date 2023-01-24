#pragma once
#include <ostream>
#include <vector>

struct Point3D {
	Point3D() {};
	Point3D(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};
	Point3D operator-(const Point3D& other);
	friend std::ostream& operator<<(std::ostream& os, const Point3D& p);
	float x, y, z;
};

struct Triangle {
	Triangle() {};
	Triangle(const Point3D& _a, const Point3D& _b, const Point3D& _c) : a(_a), b(_b), c(_c) {};
	std::vector<Point3D> points() const;
	friend std::ostream& operator<<(std::ostream& os, const Triangle& triangle);
	Point3D a, b, c;
};
