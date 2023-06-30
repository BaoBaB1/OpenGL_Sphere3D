#include <glm/glm.hpp>
#include <chrono>
#include <utility>
#include "Icosahedron.hpp"

Icosahedron::Icosahedron() : Model(Type::ICOSAHEDRON) {
	m_mesh.vertices().reserve(12);
	m_mesh.faces().reserve(20);
	float phi = (1.0f + std::sqrt(5.0f)) * 0.5f; // golden ratio
	float a = 1.0f;
	float b = a / phi;
	// 12 points
	m_mesh.append_vertex(Vertex(0.f, b, -a));
	m_mesh.append_vertex(Vertex(b, a, 0.f));
	m_mesh.append_vertex(Vertex(-b, a, 0.f));
	m_mesh.append_vertex(Vertex(0.f, b, a));
	m_mesh.append_vertex(Vertex(0.f, -b, a));
	m_mesh.append_vertex(Vertex(-a, 0.f, b));
	m_mesh.append_vertex(Vertex(0.f, -b, -a));
	m_mesh.append_vertex(Vertex(a, 0.f, -b));
	m_mesh.append_vertex(Vertex(a, 0.f, b));
	m_mesh.append_vertex(Vertex(-a, 0.f, -b));
	m_mesh.append_vertex(Vertex(b, -a, 0.f));
	m_mesh.append_vertex(Vertex(-b, -a, 0.f));
	// 20 faces
	m_mesh.append_face(Face({ 2, 1, 0 }));
	m_mesh.append_face(Face({ 1, 2, 3 }));
	m_mesh.append_face(Face({ 5, 4, 3 }));
	m_mesh.append_face(Face({ 4, 8, 3 }));
	m_mesh.append_face(Face({ 7, 6, 0 }));
	m_mesh.append_face(Face({ 6, 9, 0 }));
	m_mesh.append_face(Face({ 11, 10, 4 }));
	m_mesh.append_face(Face({ 10, 11, 6 }));
	m_mesh.append_face(Face({ 9, 5, 2 }));
	m_mesh.append_face(Face({ 5, 9, 11 }));
	m_mesh.append_face(Face({ 8, 7, 1 }));
	m_mesh.append_face(Face({ 7, 8, 10 }));
	m_mesh.append_face(Face({ 2, 5, 3 }));
	m_mesh.append_face(Face({ 8, 1, 3 }));
	m_mesh.append_face(Face({ 9, 2, 0 }));
	m_mesh.append_face(Face({ 1, 7, 0 }));
	m_mesh.append_face(Face({ 11, 9, 6 }));
	m_mesh.append_face(Face({ 7, 10, 6 }));
	m_mesh.append_face(Face({ 5, 11, 4 }));
	m_mesh.append_face(Face({ 10, 8, 4 }));
	m_cached_meshes[0] = m_mesh;
}

void Icosahedron::project_points_on_sphere() {
	for (Vertex& v : m_mesh.vertices()) {
		v.position = glm::normalize(v.position);
	}
}

void Icosahedron::subdivide_triangles(int subdivision_level, const Vertex& a, const Vertex& b, const Vertex& c) {
	if (subdivision_level == 0) {
		GLuint ind = m_mesh.append_vertex(a);
		GLuint ind2 = m_mesh.append_vertex(b);
		GLuint ind3 = m_mesh.append_vertex(c);
		m_mesh.append_face(Face({ ind, ind2, ind3 }));
		return;
	}
	Vertex ab((a + b) / 2);
	Vertex bc((b + c) / 2);
	Vertex ac((a + c) / 2);
	ab.color = bc.color = ac.color = m_color;
	// ORDER IS IMPORTANT !!! 
	subdivide_triangles(subdivision_level - 1, a, ab, ac);
	subdivide_triangles(subdivision_level - 1, b, bc, ab);
	subdivide_triangles(subdivision_level - 1, c, ac, bc);
	subdivide_triangles(subdivision_level - 1, ab, bc, ac);
}

void Icosahedron::subdivide_triangles(int subdivision_depth) {
	std::vector<Face> faces = std::move(m_mesh.faces());
	std::vector<Vertex> vertices = std::move(m_mesh.vertices());
	allocate_memory_before_subdivision(subdivision_depth);
	//auto start = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < faces.size(); ++i) {
		assert(faces[i].size == 3);
		GLuint index = faces[i].data[0];
		GLuint index2 = faces[i].data[1];
		GLuint index3 = faces[i].data[2];
		subdivide_triangles(subdivision_depth, vertices[index], vertices[index2], vertices[index3]);
	}
	auto end = std::chrono::high_resolution_clock::now();
	//std::chrono::duration<double, std::milli> ms = end - start;
	//DEBUG("Icosahedron::subdivide_triangles duration " << ms.count() << " ms\n");
	m_cached_meshes[0] = m_mesh;
}

void Icosahedron::allocate_memory_before_subdivision(int subdivision_depth) {
	m_mesh.vertices().clear();
	m_mesh.faces().clear();
	size_t numfaces = m_cached_meshes[0].faces().size() * (size_t)std::pow(4, subdivision_depth);
	size_t numvertices = numfaces * 3;
	DEBUG("preallocating memory for " << numvertices << " icosahedron points\n");
	m_mesh.vertices().reserve(numvertices);
	DEBUG("preallocating memory for " << numfaces << " icosahedron faces\n");
	m_mesh.faces().reserve(numfaces);
}
