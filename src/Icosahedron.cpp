#include <glm/glm.hpp>
#include <chrono>
#include <utility>
#include "Icosahedron.hpp"
#include "macro.hpp"

Icosahedron::Icosahedron() : Model(Type::ICOSAHEDRON) {
	generate_initial_mesh();
}

Icosahedron::Icosahedron(Icosahedron&& other) noexcept : Model(std::move(other)) {
	m_map_vert = std::move(other.m_map_vert);
}

Icosahedron& Icosahedron::operator=(Icosahedron&& other) noexcept {
	if (this != &other) {
		Model::operator=(std::move(other));
		m_map_vert = std::move(other.m_map_vert);
	}
	return *this;
}

void Icosahedron::project_points_on_sphere() {
	Mesh& m = m_meshes[0];
	for (Vertex& v : m.vertices()) {
		v.position = glm::normalize(v.position);
	}
}

void Icosahedron::apply_shading() {
	if (m_shading_mode == NO_SHADING) {
		Model::apply_shading();
	}
	else {
		for (Mesh& m : m_meshes) {
			if (!m.is_auxiliary()) {
				std::vector<Vertex>& vertices = m.vertices();
				std::vector<Face>& faces = m.faces();
				for (const Face& face : faces) {
					assert(face.size == 3);  // for now only triangle faces
					GLuint ind = face.data[0], ind2 = face.data[1], ind3 = face.data[2];
					glm::vec3 a = vertices[ind2].position - vertices[ind].position;
					glm::vec3 b = vertices[ind3].position - vertices[ind].position;
					glm::vec3 cross = glm::cross(a, b);
					if (m_shading_mode == SMOOTH_SHADING) {
						vertices[ind].normal += cross;
						vertices[ind2].normal += cross;
						vertices[ind3].normal += cross;
					}
					else {
						vertices[ind].normal = cross;
						vertices[ind2].normal = cross;
						vertices[ind3].normal = cross;
					}
				}
				for (Vertex& v : vertices) {
					v.normal = glm::normalize(v.normal);
				}
			}
		}
	}
}

void Icosahedron::subdivide_triangles(int subdivision_level, const Vertex& a, const Vertex& b, const Vertex& c) {
	if (subdivision_level == 0) {
		Mesh& mesh = m_meshes[0];
		GLuint ind[3] = {};
		Vertex tri[3] = { a, b, c };
		for (int i = 0; i < 3; ++i) {
			// each face (triangle) has it's own normal, so duplicate vertices of each face
			if (m_shading_mode == ShadingMode::FLAT_SHADING) {
				ind[i] = mesh.append_vertex(tri[i]);
			}
			else {
				// for smooth shading we have to make sure that every vertex is unique as well as it's normal.
				// fragment color will be interpolated between triangle's vertex normals
				auto vert = m_map_vert.find(tri[i]);
				if (vert != m_map_vert.end()) {
					ind[i] = vert->second;
				}
				else {
					ind[i] = mesh.append_vertex(tri[i]);
					m_map_vert.insert(std::make_pair(tri[i], ind[i]));
				}
			}
		}
		mesh.append_face(Face({ ind[0], ind[1], ind[2] }));
		return;
	}
	Vertex ab((a + b) / 2);
	Vertex bc((b + c) / 2);
	Vertex ac((a + c) / 2);
	// ORDER IS IMPORTANT !!! CW ORDER 
	subdivide_triangles(subdivision_level - 1, a, ab, ac);
	subdivide_triangles(subdivision_level - 1, b, bc, ab);
	subdivide_triangles(subdivision_level - 1, c, ac, bc);
	subdivide_triangles(subdivision_level - 1, ab, bc, ac);
}

void Icosahedron::subdivide_triangles(size_t subdivision_depth) {
	m_meshes[0].faces().clear();
	m_meshes[0].vertices().clear();
	m_map_vert.clear();
	allocate_memory_before_subdivision(subdivision_depth);
	const Mesh& initial_mesh = m_meshes[1];
	const std::vector<Face>& initial_mesh_faces = initial_mesh.faces();
	const std::vector<Vertex>& initial_mesh_vertices = initial_mesh.vertices();
	auto start = std::chrono::high_resolution_clock::now();
	for (size_t j = 0; j < initial_mesh_faces.size(); ++j) {
		assert(initial_mesh_faces[j].size == 3);
		GLuint index = initial_mesh_faces[j].data[0];
		GLuint index2 = initial_mesh_faces[j].data[1];
		GLuint index3 = initial_mesh_faces[j].data[2];
		subdivide_triangles(subdivision_depth, initial_mesh_vertices[index], initial_mesh_vertices[index2], initial_mesh_vertices[index3]);
	}
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> ms = end - start;
	DEBUG("Icosahedron::subdivide_triangles duration " << ms.count() << " ms\n");
}

size_t Icosahedron::faces_count_after_subdivision(size_t subdivision_depth) const {
	size_t faces = m_meshes[1].faces().size(); // initial
	for (size_t i = 0; i < subdivision_depth; ++i) {
		// each subdivision gives 4 new triangles
		faces *= 4;
	}
	return faces;
}

size_t Icosahedron::vertices_count_after_subdivision(size_t subdivision_depth) const {
	if (m_shading_mode == FLAT_SHADING)
		return faces_count_after_subdivision(subdivision_depth) * 3;
	else 
		return faces_count_after_subdivision(subdivision_depth) / 2 + 2;
}

void Icosahedron::allocate_memory_before_subdivision(const size_t subdivision_depth) {
	size_t face_count = faces_count_after_subdivision(subdivision_depth);
	size_t vertex_count = vertices_count_after_subdivision(subdivision_depth);
	std::vector<Face>& main_mesh_faces = m_meshes[0].faces();
	std::vector<Vertex>& main_mesh_vertices = m_meshes[0].vertices();
	if (main_mesh_vertices.capacity() < vertex_count) {
		DEBUG("preallocating memory for " << vertex_count << " icosahedron points\n");
		main_mesh_vertices.reserve(vertex_count);
	}
	if (main_mesh_faces.capacity() < face_count) {
		DEBUG("preallocating memory for " << face_count << " icosahedron faces\n");
		main_mesh_faces.reserve(face_count);
	}
	if (m_shading_mode == SMOOTH_SHADING)
		m_map_vert.reserve(vertex_count);
}

void Icosahedron::generate_initial_mesh() {
	m_meshes.resize(2);
	m_meshes[1].vertices().reserve(12);
	m_meshes[1].faces().reserve(20);
	float phi = (1.0f + std::sqrt(5.0f)) * 0.5f; // golden ratio
	float a = 1.0f;
	float b = a / phi;
	// 12 points
	Mesh& initial_mesh = m_meshes[1];
	initial_mesh.set_auxiliary(true);
	initial_mesh.append_vertex(Vertex(0.f, b, -a));
	initial_mesh.append_vertex(Vertex(b, a, 0.f));
	initial_mesh.append_vertex(Vertex(-b, a, 0.f));
	initial_mesh.append_vertex(Vertex(0.f, b, a));
	initial_mesh.append_vertex(Vertex(0.f, -b, a));
	initial_mesh.append_vertex(Vertex(-a, 0.f, b));
	initial_mesh.append_vertex(Vertex(0.f, -b, -a));
	initial_mesh.append_vertex(Vertex(a, 0.f, -b));
	initial_mesh.append_vertex(Vertex(a, 0.f, b));
	initial_mesh.append_vertex(Vertex(-a, 0.f, -b));
	initial_mesh.append_vertex(Vertex(b, -a, 0.f));
	initial_mesh.append_vertex(Vertex(-b, -a, 0.f));
	// 20 faces
	initial_mesh.append_face(Face({ 2, 1, 0 }));
	initial_mesh.append_face(Face({ 1, 2, 3 }));
	initial_mesh.append_face(Face({ 5, 4, 3 }));
	initial_mesh.append_face(Face({ 4, 8, 3 }));
	initial_mesh.append_face(Face({ 7, 6, 0 }));
	initial_mesh.append_face(Face({ 6, 9, 0 }));
	initial_mesh.append_face(Face({ 11, 10, 4 }));
	initial_mesh.append_face(Face({ 10, 11, 6 }));
	initial_mesh.append_face(Face({ 9, 5, 2 }));
	initial_mesh.append_face(Face({ 5, 9, 11 }));
	initial_mesh.append_face(Face({ 8, 7, 1 }));
	initial_mesh.append_face(Face({ 7, 8, 10 }));
	initial_mesh.append_face(Face({ 2, 5, 3 }));
	initial_mesh.append_face(Face({ 8, 1, 3 }));
	initial_mesh.append_face(Face({ 9, 2, 0 }));
	initial_mesh.append_face(Face({ 1, 7, 0 }));
	initial_mesh.append_face(Face({ 11, 9, 6 }));
	initial_mesh.append_face(Face({ 7, 10, 6 }));
	initial_mesh.append_face(Face({ 5, 11, 4 }));
	initial_mesh.append_face(Face({ 10, 8, 4 }));
	m_meshes[0] = m_meshes[1];
	m_meshes[0].set_auxiliary(false);
}

UpDownNumberChanger::UpDownNumberChanger(int min_value, int max_value) {
	if (min_value > max_value)
		throw std::runtime_error("Max value < min_value");
	m_min_value = min_value;
	m_max_value = max_value;
	m_value = min_value;
	m_increase_value = true;
}
int UpDownNumberChanger::next_value() {
	if (m_min_value == m_max_value)
		return m_value;
	if ((m_value == m_max_value && m_increase_value) || (m_value == m_min_value && !m_increase_value)) {
		m_increase_value = !m_increase_value;
	}
	m_increase_value ? ++m_value : --m_value;
	return m_value;
}
