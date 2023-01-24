#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GeometryPrimitives.h"
#include <array>

class Icosahedron {
public:
	Icosahedron();
	Icosahedron(Icosahedron&& other);
	Icosahedron& operator=(Icosahedron&& other); 
	std::vector<Point3D> mesh() const;
	std::vector<GLuint> indices() const;
	size_t indices_count_after_subdivision(const size_t subdivision_depth);
	size_t points_count_after_subdivision(const size_t subdivision_depth);
	size_t append_vertex(const Point3D& point);
	void add_indices(const GLuint i, const GLuint i2, const GLuint i3);
	void subdivide_triangles(const size_t subdivision_depth);
	void project_points_on_sphere();
private:
	void subdivide_triangles(const size_t subdivision_level, const Point3D& a, const Point3D& b, const Point3D& c,
							 const GLuint index_a, const GLuint index_b, const GLuint index_c);
	void add_initial_indices(const GLuint i, const GLuint i2, const GLuint i3, const size_t triangle);
	void allocate_memory_before_subdivision(const size_t subdivision_depth);
	void generate_mesh();
	void generate_faces();
private:
	std::vector<Point3D> m_mesh;
	std::vector<GLuint> m_indices;
	std::array<Point3D, 12> m_initial_mesh;
	std::array<GLuint, 60> m_initial_indices;
};