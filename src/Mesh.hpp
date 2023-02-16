#pragma once

#include <vector>
#include <glad/glad.h>
#include "Vertex.hpp"
#include "Face.hpp"

class Mesh {
public:
	Mesh() : m_is_auxiliary(false) {}
	Mesh(const std::vector<Vertex>& vertices, const std::vector<Face>& faces);
	Mesh(const Mesh&) = default;
	Mesh& operator=(const Mesh&) = default;
	Mesh(Mesh&& other) noexcept = default;
	Mesh& operator=(Mesh&& other) = default;
	~Mesh() noexcept {}

	std::vector<Vertex>& vertices() { return m_vertices; }
	const std::vector<Vertex>& vertices() const { return m_vertices; }
	std::vector<Face>& faces() { return m_faces; }
	const std::vector<Face>& faces() const { return m_faces; }
	bool is_auxiliary() const { return m_is_auxiliary; }
	void set_auxiliary(bool val) { m_is_auxiliary = val; }
	void faces_as_indices(std::vector<GLuint>& buffer) const;
	void reserve_vertices(size_t size) { m_vertices.reserve(size); }
	void reserve_faces(size_t size) { m_faces.reserve(size); }
	size_t append_vertex(const Vertex& vertex);
	size_t append_face(const Face& face);
	size_t append_face(Face&& face);
private:
	std::vector<Vertex> m_vertices;
	std::vector<Face> m_faces;
	bool m_is_auxiliary;
};
