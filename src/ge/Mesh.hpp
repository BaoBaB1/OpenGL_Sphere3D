#pragma once

#include <vector>
#include <optional>
#include <glad/gl.h>
#include <memory>
#include "ge/Vertex.hpp"
#include "ge/Face.hpp"
#include "ge/BoundingBox.hpp"
#include "core/Texture2D.hpp"

class Mesh {
public:
  Mesh() = default;
  Mesh(const std::vector<Vertex>& vertices, const std::vector<Face>& faces);
  std::vector<Vertex>& vertices() { return m_vertices; }
  const std::vector<Vertex>& vertices() const { return m_vertices; }
  std::vector<Face>& faces() { return m_faces; }
  const std::vector<Face>& faces() const { return m_faces; }
  std::vector<GLuint> faces_as_indices() const;
  std::shared_ptr<Texture2D>& texture() { return m_texture; }
  const std::shared_ptr<Texture2D>& texture() const { return m_texture; }
  BoundingBox& bbox() { return m_bbox; }
  const BoundingBox& bbox() const { return m_bbox; }
  size_t append_vertex(const Vertex& vertex);
  size_t append_face(const Face& face);
  size_t append_face(Face&& face);
  friend class Object3D;
private:
  std::vector<Vertex> m_vertices;
  std::vector<Face> m_faces;
  std::shared_ptr<Texture2D> m_texture;
  std::vector<Vertex> m_cached_normals;     // normal lines
  BoundingBox m_bbox;
};
