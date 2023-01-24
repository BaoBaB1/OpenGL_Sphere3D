#include <glm/glm.hpp>
#include <chrono>
#include "Icosahedron.h"
#include "macro.h"

using namespace std::chrono;

Icosahedron::Icosahedron() {
	generate_mesh();
	generate_faces();
}

Icosahedron::Icosahedron(Icosahedron&& other) {
	*this = std::move(other);
}

Icosahedron& Icosahedron::operator=(Icosahedron&& other) {
	if (this == &other)
		return *this;
	m_mesh = std::move(other.m_mesh);
	m_indices = std::move(other.m_indices);
	m_initial_mesh = std::move(other.m_initial_mesh);
	m_initial_indices = std::move(other.m_initial_indices);
	return *this;
}

void Icosahedron::project_points_on_sphere() {
	for (size_t i = 0; i < m_mesh.size(); i++) {
		glm::vec3 normalized_v = glm::normalize(glm::vec3(m_mesh[i].x, m_mesh[i].y, m_mesh[i].z));
		m_mesh[i].x = normalized_v.x;
		m_mesh[i].y = normalized_v.y;
		m_mesh[i].z = normalized_v.z;
	}	
}

void Icosahedron::subdivide_triangles(const size_t subdivision_level, const Point3D& a, const Point3D& b, const Point3D& c, 
									  const GLuint index_a, const GLuint index_b, const GLuint index_c) {
	if (subdivision_level == 0) {
		add_indices(index_a, index_b, index_c);
		return;
	}
	Point3D ab((a.x + b.x) / 2, (a.y + b.y) / 2, (a.z + b.z) / 2);
	Point3D bc((b.x + c.x) / 2, (b.y + c.y) / 2, (b.z + c.z) / 2);
	Point3D ac((a.x + c.x) / 2, (a.y + c.y) / 2, (a.z + c.z) / 2);
	GLuint ind_ab = (GLuint)append_vertex(ab);
	GLuint ind_bc = (GLuint)append_vertex(bc);
	GLuint ind_ac = (GLuint)append_vertex(ac);
	subdivide_triangles(subdivision_level - 1, a, ab, ac, index_a, ind_ab, ind_ac);
	subdivide_triangles(subdivision_level - 1, b, ab, bc, index_b, ind_ab, ind_bc);
	subdivide_triangles(subdivision_level - 1, c, ac, bc, index_c, ind_ac, ind_bc);
	subdivide_triangles(subdivision_level - 1, ab, bc, ac, ind_ab, ind_bc, ind_ac);
}

void Icosahedron::subdivide_triangles(const size_t subdivision_depth) {
	m_indices.clear();
	m_mesh.clear();
	if (subdivision_depth == 0)
		return;
	const size_t n_triangles = m_initial_indices.size() / 3;
	allocate_memory_before_subdivision(subdivision_depth);
	for (size_t j = 0; j < n_triangles; ++j) {
		GLuint index = m_initial_indices[j * 3];
		GLuint index2 = m_initial_indices[j * 3 + 1];
		GLuint index3 = m_initial_indices[j * 3 + 2];
		subdivide_triangles(subdivision_depth, m_initial_mesh[index], m_initial_mesh[index2], m_initial_mesh[index3], index, index2, index3);
	}
}

size_t Icosahedron::indices_count_after_subdivision(const size_t subdivision_depth) {
	size_t n_triangles = m_initial_indices.size() / 3;
	size_t n_indices = 0;
	size_t n_indices_prev = 0; 
	for (size_t i = 0; i < subdivision_depth; ++i) {
		n_indices = n_triangles * 4 * 3 - n_indices_prev;
		n_triangles += n_indices / 3;
		n_indices_prev += n_indices;
	}
	return n_indices;
}

size_t Icosahedron::points_count_after_subdivision(const size_t subdivision_depth) {
	size_t n_points = 0;
	for (size_t i = subdivision_depth; i > 0; --i) {
		// each subdivision gives 4 new triangles and 3 new points
		n_points += indices_count_after_subdivision(i) / 4;
	}
	return n_points + m_initial_mesh.size();
}

void Icosahedron::add_initial_indices(const GLuint i, const GLuint i2, const GLuint i3, const size_t triangle) {
	m_initial_indices[triangle * 3] = i;
	m_initial_indices[triangle * 3 + 1] = i2;
	m_initial_indices[triangle * 3 + 2] = i3;
}

void Icosahedron::allocate_memory_before_subdivision(const size_t subdivision_depth) {
	size_t indices_count = indices_count_after_subdivision(subdivision_depth);
	size_t points_count = points_count_after_subdivision(subdivision_depth);
	//DEBUG("preallocating memory for " << points_count << " points" << std::endl);
	//DEBUG("preallocating memory for " << indices_count << " indices" << std::endl);
	m_mesh.reserve(points_count);
	m_indices.reserve(indices_count);
	for (const Point3D& p : m_initial_mesh) { // indices no need to be copied unlike points
		m_mesh.push_back(p);
	}
}

std::vector<Point3D> Icosahedron::mesh() const { 
	if (!m_mesh.empty())
		return m_mesh; 
	return std::vector<Point3D>(m_initial_mesh.data(), m_initial_mesh.data() + m_initial_mesh.size());
}

std::vector<GLuint> Icosahedron::indices() const { 
	if (!m_indices.empty())
		return m_indices; 
	return std::vector<GLuint>(m_initial_indices.data(), m_initial_indices.data() + m_initial_indices.size());
}

size_t Icosahedron::append_vertex(const Point3D& vertex) {
	m_mesh.push_back(vertex);
	return m_mesh.size() - 1;
}

void Icosahedron::add_indices(const GLuint i, const GLuint i2, const GLuint i3) {
	m_indices.push_back(i);
	m_indices.push_back(i2);
	m_indices.push_back(i3);
}

void Icosahedron::generate_mesh() {
	float phi = (1.0f + std::sqrt(5.0f)) * 0.5f; // golden ratio
	float a = 1.0f;
	float b = a / phi;
	// 12 points
	size_t i = 0;
	m_initial_mesh[i++] = Point3D(0.f, b, -a);
	m_initial_mesh[i++] = Point3D(b, a, 0.f);
	m_initial_mesh[i++] = Point3D(-b, a, 0.f);
	m_initial_mesh[i++] = Point3D(0.f, b, a);
	m_initial_mesh[i++] = Point3D(0.f, -b, a);
	m_initial_mesh[i++] = Point3D(-a, 0.f, b);
	m_initial_mesh[i++] = Point3D(0.f, -b, -a);
	m_initial_mesh[i++] = Point3D(a, 0.f, -b);
	m_initial_mesh[i++] = Point3D(a, 0.f, b);
	m_initial_mesh[i++] = Point3D(-a, 0.f, -b);
	m_initial_mesh[i++] = Point3D(b, -a, 0.f);
	m_initial_mesh[i++] = Point3D(-b, -a, 0.f);
}

void Icosahedron::generate_faces() {
	// 20 faces
	size_t triangle = 0;
	add_initial_indices(2, 1, 0, triangle++);
	add_initial_indices(1, 2, 3, triangle++);
	add_initial_indices(5, 4, 3, triangle++);
	add_initial_indices(4, 8, 3, triangle++);
	add_initial_indices(7, 6, 0, triangle++);
	add_initial_indices(6, 9, 0, triangle++);
	add_initial_indices(11, 10, 4, triangle++);
	add_initial_indices(10, 11, 6, triangle++);
	add_initial_indices(9, 5, 2, triangle++);
	add_initial_indices(5, 9, 11, triangle++);
	add_initial_indices(8, 7, 1, triangle++);
	add_initial_indices(7, 8, 10, triangle++);
	add_initial_indices(2, 5, 3, triangle++);
	add_initial_indices(8, 1, 3, triangle++);
	add_initial_indices(9, 2, 0, triangle++);
	add_initial_indices(1, 7, 0, triangle++);
	add_initial_indices(11, 9, 6, triangle++);
	add_initial_indices(7, 10, 6, triangle++);
	add_initial_indices(5, 11, 4, triangle++);
	add_initial_indices(10, 8, 4, triangle++);
}
