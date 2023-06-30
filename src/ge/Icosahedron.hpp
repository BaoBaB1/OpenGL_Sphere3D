#pragma once

#include <unordered_map>
#include "Model.hpp"

class Icosahedron : public Model {
public:
	Icosahedron();
	void subdivide_triangles(int subdivision_depth);
	void project_points_on_sphere();
private:
	void subdivide_triangles(int subdivision_level, const Vertex& a, const Vertex& b, const Vertex& c);
	void allocate_memory_before_subdivision(int subdivision_depth);
};
