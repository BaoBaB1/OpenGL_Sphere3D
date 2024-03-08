#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "./core/Shader.hpp"
#include "./core/GPUBuffers.hpp"
#include "./core/Texture.hpp"
#include "./ge/IDrawable.hpp"
#include "./ge/Mesh.hpp"
#include "./ge/BoundingBox.hpp"

class Object3D : public IDrawable
{
public:
  virtual void set_color(const glm::vec4& color);
  virtual void set_texture(const std::string& filename);
  void rotate(float angle, const glm::vec3& axis);
  void scale(const glm::vec3& scale);
  void translate(const glm::vec3& translation);
  void set_bbox(const BoundingBox& bbox) { m_bbox = bbox; }
  std::vector<Vertex> normals_as_lines();
  BoundingBox calculate_bbox();
  float rotation_angle() const { return m_rotation_angle; }
  glm::vec3 rotation_axis() const { return m_rotation_axis; }
  glm::vec3 translation() const { return m_translation; }
  glm::vec3 scale() const { return m_scale; }
  void light_source(bool val) { set_flag(LIGHT_SOURCE, val); }
  void rotating(bool val) { set_flag(ROTATE_EACH_FRAME, val); }
  void visible_normals(bool val) { set_flag(VISIBLE_NORMALS, val); }
  void visible_bbox(bool val) { return set_flag(VISIBLE_BBOX, val); }
  bool is_normals_visible() const { return get_flag(VISIBLE_NORMALS); }
  bool is_rotating() const { return get_flag(ROTATE_EACH_FRAME); }
  bool is_light_source() const { return get_flag(LIGHT_SOURCE); }
  bool is_bbox_visible() const { return get_flag(VISIBLE_BBOX); }
  const glm::mat4& model_matrix() const { return m_model_mat; }
  const glm::vec4& color() const { return m_color; }
  Mesh& mesh() { return m_mesh; }
  const Mesh& mesh() const { return m_mesh; }
  Texture& texture() { return m_texture; }
  const Texture& texture() const { return m_texture; }
  BoundingBox& bbox() { return m_bbox; }
  const BoundingBox& bbox() const { return m_bbox; }
  friend class SceneRenderer;
protected:
  enum Flag 
  {
    ROTATE_EACH_FRAME = (1 << 0),
    VISIBLE_NORMALS = (1 << 1),
    LIGHT_SOURCE = (1 << 2),
    VISIBLE_BBOX = (1 << 3)
  };
  struct RenderConfig
  {
    int mode;
    bool use_indices;
    bool apply_shading;
  };
protected:
  Object3D();
  void render(GPUBuffers*, Shader*, RenderConfig*);
  void set_flag(Flag flag, bool value) { value ? set_flag(flag) : clear_flag(flag); }
  void set_flag(Flag flag) { m_flags |= flag; }
  void clear_flag(Flag flag) { m_flags &= ~flag; }
  bool get_flag(Flag flag) const { return m_flags & flag; }
protected:
  Mesh m_mesh;
  Texture m_texture;
  BoundingBox m_bbox;
  glm::mat4 m_model_mat;
  glm::vec4 m_color;
  float m_rotation_angle;
  glm::vec3 m_rotation_axis;
  glm::vec3 m_translation;
  glm::vec3 m_scale;
  int m_flags;
};
