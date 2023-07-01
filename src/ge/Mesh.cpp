#include "Mesh.hpp"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<Face>& faces) {
  m_vertices = vertices;
  m_faces = faces;
}

size_t Mesh::append_vertex(const Vertex& vertex) {
  m_vertices.push_back(vertex);
  return m_vertices.size() - 1;
}

size_t Mesh::append_face(const Face& face) {
  m_faces.push_back(face);
  return m_faces.size() - 1;
}

size_t Mesh::append_face(Face&& face) {
  m_faces.push_back(std::move(face));
  return m_faces.size() - 1;
}

std::vector<GLuint> Mesh::faces_as_indices() const {
  std::vector<GLuint> buffer;
  size_t n_indices = 0;
  for (size_t i = 0; i < m_faces.size(); ++i) {
    n_indices += m_faces[i].size;
  }
  buffer.resize(n_indices);
  n_indices = 0;
  for (size_t i = 0; i < m_faces.size(); ++i) {
    memcpy(buffer.data() + n_indices, m_faces[i].data, sizeof(GLuint) * m_faces[i].size);
    n_indices += m_faces[i].size;
  }
  return buffer;
}
