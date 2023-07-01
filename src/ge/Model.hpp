#pragma once

#include <array>
#include <unordered_map>
#include "./ge/Texture.hpp"
#include "./core/Shader.hpp"
#include "./core/macro.hpp"
#include "./core/GPUBuffers.hpp"
#include "IShaderable.hpp"
#include "Object3D.hpp"

class Model : public Object3D, public IShaderable {
public:
  enum Type {
    ICOSAHEDRON = 1, // Icosahedron => Sphere
    CUBE,
    PYRAMID,
  };
public:
  virtual void set_texture(const std::string& filename);
  void set_color(const glm::vec4& color) override;
  void render(GPUBuffers*, Shader*) override;
  void apply_shading(IShaderable::ShadingMode mode) override;
  std::vector<Vertex> normals_as_lines();
  glm::vec3 center();
  Texture& texture() { return m_texture; }
  const Texture& texture() const { return m_texture; }
  Type type() const { return m_type; }
  ShadingMode shading_mode() const { return m_shading_mode; }
  void light_source(bool val) { set_flag(LIGHT_SOURCE, val); }
  void rotating(bool val) { set_flag(ROTATE_EACH_FRAME, val); }
  void visible_normals(bool val) { set_flag(VISIBLE_NORMALS, val); }
  bool is_normals_visible() const { return get_flag(VISIBLE_NORMALS); }
  bool is_rotating() const { return get_flag(ROTATE_EACH_FRAME); }
  bool is_light_source() const { return get_flag(LIGHT_SOURCE); }
protected:
  enum Flag {
    ROTATE_EACH_FRAME = (1 << 0),
    VISIBLE_NORMALS = (1 << 1),
    LIGHT_SOURCE = (1 << 2),
  };
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
  void set_flag(Flag flag, bool value) { value ? set_flag(flag) : clear_flag(flag); }
  void set_flag(Flag flag) { m_flags |= flag; }
  void clear_flag(Flag flag) { m_flags &= ~flag; }
  bool get_flag(Flag flag) const { return m_flags & flag; }
  void calc_normals(Mesh&, IShaderable::ShadingMode);
protected:
  Type m_type;
  Texture m_texture;
  IShaderable::ShadingMode m_shading_mode;
  unsigned int m_flags;
  VertexFinder m_vertex_finder;
  std::array<Mesh, 3> m_cached_meshes;
};
