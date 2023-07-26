#pragma once

#include <array>
#include <unordered_map>
#include "IShaderable.hpp"
#include "Object3D.hpp"

class Model : public Object3D, public IShaderable 
{
public:
  enum Type 
  {
    ICOSAHEDRON = 1, // Icosahedron => Sphere
    CUBE,
    PYRAMID
  };
public:
  bool has_surface() const override { return true; }
  void set_color(const glm::vec4& color) override;
  void render(GPUBuffers*, Shader*) override;
  void apply_shading(IShaderable::ShadingMode mode) override;
  glm::vec3 center();
  Type type() const { return m_type; }
  ShadingMode shading_mode() const { return m_shading_mode; }
protected:
  // Wrapper for Vertex. Vertex::operator== compares not only position
  struct WrappedVertex {
    explicit WrappedVertex(const Vertex& vertex) {
      this->vertex = vertex;
    }
    bool operator==(const WrappedVertex& other) const { return other.vertex.position == this->vertex.position; }
    Vertex vertex;
  };
  struct VertexHasher {
    size_t operator()(const WrappedVertex& wrapped) const {
      std::hash<float> hasher;
      return hasher(wrapped.vertex.position.x) + hasher(wrapped.vertex.position.y) ^ hasher(wrapped.vertex.position.z);
    }
  };
  struct VertexFinder {
    using iter = std::unordered_map<WrappedVertex, GLuint, VertexHasher, std::equal_to<WrappedVertex>>::iterator;
    iter end() { return m_map_vert.end(); }
    iter find_vertex(const Vertex& v) {
      return m_map_vert.find(WrappedVertex(v));
    }
    void add_vertex(const Vertex& v, GLuint index) {
      m_map_vert.insert(std::make_pair(WrappedVertex(v), index));
    }
    std::unordered_map<WrappedVertex, GLuint, VertexHasher, std::equal_to<WrappedVertex>> m_map_vert; // vertex, index
  };
protected:
  explicit Model(Type type);
  Model(const Model&) = default;
  Model(Model&&) = default;
  Model& operator=(const Model&) = default;
  Model& operator=(Model&&) = default;
  void calc_normals(Mesh&, IShaderable::ShadingMode);
protected:
  Type m_type;
  IShaderable::ShadingMode m_shading_mode;
  //unsigned int m_flags;
  VertexFinder m_vertex_finder;
  std::array<Mesh, 3> m_cached_meshes;
};
