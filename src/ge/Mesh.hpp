#pragma once

#include <vector>
#include <glad/glad.h>
#include "ge/Vertex.hpp"
#include "ge/Face.hpp"

class Mesh {
public:
  Mesh() = default;
  Mesh(const std::vector<Vertex>& vertices, const std::vector<Face>& faces);
  std::vector<Vertex>& vertices() { return m_vertices; }
  const std::vector<Vertex>& vertices() const { return m_vertices; }
  std::vector<Face>& faces() { return m_faces; }
  const std::vector<Face>& faces() const { return m_faces; }
  std::vector<GLuint> faces_as_indices() const;
  void reserve_vertices(size_t size) { m_vertices.reserve(size); }
  void reserve_faces(size_t size) { m_faces.reserve(size); }
  void resize_vertices(size_t size) { m_vertices.resize(size); }
  void resize_faces(size_t size) { m_faces.resize(size); }
  size_t append_vertex(const Vertex& vertex);
  size_t append_face(const Face& face);
  size_t append_face(Face&& face);
private:
  std::vector<Vertex> m_vertices;
  std::vector<Face> m_faces;
};
