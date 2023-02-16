#include "Cube.hpp"
#include <map>

Cube::Cube() : Model(Type::CUBE) {
	generate_initial_mesh();
}

Cube::Cube(Cube&& other) noexcept : Model(std::move(other)) {
	m_cached_normals = std::move(other.m_cached_normals);
}

Cube& Cube::operator=(Cube&& other) noexcept {
	if (this != &other) {
		Model::operator=(std::move(other));
		m_cached_normals = std::move(other.m_cached_normals);
	}
	return *this;
}

void Cube::generate_initial_mesh() {
	Mesh mesh;
	const int N = 24;
	m_cached_normals.resize(N);
	mesh.reserve_vertices(N);
	mesh.reserve_faces(36);
	// each vertex belongs to 3 faces. for correct normals we have to duplicate each vertex 3 times
	// front face
	mesh.append_vertex(Vertex(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f))); // bottom left
	mesh.append_vertex(Vertex(glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f))); // bottom right
	mesh.append_vertex(Vertex(glm::vec3(1.f, 1.f, 0.f), glm::vec3(0.f, 0.f, -1.f))); // top right
	mesh.append_vertex(Vertex(glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, -1.f))); // top left
	// back face		 
	mesh.append_vertex(Vertex(glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 1.f))); // bottom left
	mesh.append_vertex(Vertex(glm::vec3(1.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 1.f))); // bottom right
	mesh.append_vertex(Vertex(glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.f, 0.f, 1.f))); // top right
	mesh.append_vertex(Vertex(glm::vec3(0.f, 1.f, 1.f), glm::vec3(0.f, 0.f, 1.f))); // top left
	// front face
	mesh.append_vertex(Vertex(glm::vec3(0.f, 0.f, 0.f), glm::vec3(-1.f, 0.f, 0.f))); // bottom left
	mesh.append_vertex(Vertex(glm::vec3(1.f, 0.f, 0.f), glm::vec3(1.f, 0.f, 0.f))); // bottom right
	mesh.append_vertex(Vertex(glm::vec3(1.f, 1.f, 0.f), glm::vec3(1.f, 0.f, 0.f))); // top right
	mesh.append_vertex(Vertex(glm::vec3(0.f, 1.f, 0.f), glm::vec3(-1.f, 0.f, 0.f))); // top left
	// back face
	mesh.append_vertex(Vertex(glm::vec3(0.f, 0.f, 1.f), glm::vec3(-1.f, 0.f, 0.f))); // bottom left
	mesh.append_vertex(Vertex(glm::vec3(1.f, 0.f, 1.f), glm::vec3(1.f, 0.f, 0.f))); // bottom right
	mesh.append_vertex(Vertex(glm::vec3(1.f, 1.f, 1.f), glm::vec3(1.f, 0.f, 0.f))); // top right
	mesh.append_vertex(Vertex(glm::vec3(0.f, 1.f, 1.f), glm::vec3(-1.f, 0.f, 0.f))); // top left
	// front face
	mesh.append_vertex(Vertex(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f))); // bottom left
	mesh.append_vertex(Vertex(glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f))); // bottom right
	mesh.append_vertex(Vertex(glm::vec3(1.f, 1.f, 0.f), glm::vec3(0.f, 1.f, 0.f))); // top right
	mesh.append_vertex(Vertex(glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 1.f, 0.f))); // top left
	// back face
	mesh.append_vertex(Vertex(glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, -1.f, 0.f))); // bottom left
	mesh.append_vertex(Vertex(glm::vec3(1.f, 0.f, 1.f), glm::vec3(0.f, -1.f, 0.f))); // bottom right
	mesh.append_vertex(Vertex(glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.f, 1.f, 0.f))); // top right
	mesh.append_vertex(Vertex(glm::vec3(0.f, 1.f, 1.f), glm::vec3(0.f, 1.f, 0.f))); // top left

	// make sure all indices have the same normals
	// v[0], v[1], v[2] -> 0 0 -1
	// v[8], v[12], v[15] -> -1 0 0 
	// ... 
	// v[19], v[22], v[23] -> 0 1 0
	mesh.append_face(Face{ 0, 1, 2 });
	mesh.append_face(Face{ 0, 2, 3 });
	mesh.append_face(Face{ 8, 12, 15 });
	mesh.append_face(Face{ 8, 15, 11 });
	mesh.append_face(Face{ 4, 5, 6 });
	mesh.append_face(Face{ 4, 6, 7 });
	mesh.append_face(Face{ 9, 13, 14 });
	mesh.append_face(Face{ 9, 14, 10 });
	mesh.append_face(Face{ 16, 17, 21 });
	mesh.append_face(Face{ 16, 21, 20 });
	mesh.append_face(Face{ 19, 18, 22 });
	mesh.append_face(Face{ 19, 22, 23 });
	m_meshes.push_back(mesh);
	for (int i = 0; i < N; ++i) {
		m_cached_normals[i] = mesh.vertices()[i].normal;
	}
}

void Cube::apply_shading() {
	std::vector<Vertex>& vertices = m_meshes[0].vertices();
	assert(vertices.size() == 24);
	if (m_shading_mode == SMOOTH_SHADING) {
		const int N = 8;
		for (int i = 0; i < N; ++i) {
			glm::vec3 sum = glm::normalize(vertices[i].normal + vertices[i + N].normal + vertices[i + 2 * N].normal);
			vertices[i].normal = sum;
			vertices[i + N].normal = sum;
			vertices[i + 2 * N].normal = sum;
		}
	}
	else if (m_shading_mode == FLAT_SHADING) {
		for (int i = 0; i < vertices.size(); ++i) {
			vertices[i].normal = m_cached_normals[i];
		}
	}
	else {
		Model::apply_shading();
	}
}

glm::vec3 Cube::center() const {
	return m_model_mat * glm::vec4(0.5, 0.5f, 0.5f, 1.f); // to world space
}
