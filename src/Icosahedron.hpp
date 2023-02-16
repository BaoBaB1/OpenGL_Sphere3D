#pragma once

#include <unordered_map>
#include "Model.hpp"
#include "macro.hpp"

class Icosahedron : public Model {
public:
	Icosahedron();
	Icosahedron(const Icosahedron&) = default;
	Icosahedron& operator=(const Icosahedron&) = default;
	Icosahedron(Icosahedron&&) noexcept;
	Icosahedron& operator=(Icosahedron&&) noexcept; 
	~Icosahedron() {}
	virtual void apply_shading() override;
	size_t faces_count_after_subdivision(size_t subdivision_depth) const;
	size_t vertices_count_after_subdivision(size_t subdivision_depth) const;
	void subdivide_triangles(size_t subdivision_depth);
	void project_points_on_sphere();
private:
	void subdivide_triangles(int subdivision_level, const Vertex& a, const Vertex& b, const Vertex& c);
	void allocate_memory_before_subdivision(size_t subdivision_depth);
	void generate_initial_mesh() override;
private:
	struct KeyHasher {
		size_t operator()(const Vertex& v) const {
			std::hash<float> hasher;
			return hasher(v.position.x) + hasher(v.position.y) ^ hasher(v.position.z);;
		}
	};
	std::unordered_map<Vertex, GLuint, KeyHasher, std::equal_to<Vertex>> m_map_vert; // vertex, index
};

class UpDownNumberChanger {
public:
	UpDownNumberChanger(int min_value, int max_value);
	int next_value();
	int value() { return m_value; }
	int max_value() { return m_max_value; }
	int min_value() { return m_min_value; }
private:
	int m_value;
	int m_min_value;
	int m_max_value;
	bool m_increase_value;
};
