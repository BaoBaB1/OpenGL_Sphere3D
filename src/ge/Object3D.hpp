#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "./ge/IDrawable.hpp"
#include "./ge/Mesh.hpp"

class Object3D : public IDrawable {
public:
  virtual void set_color(const glm::vec4& color);
  void rotate(float angle, const glm::vec3& axis) { m_model_mat = glm::rotate(m_model_mat, glm::radians(angle), axis); }
  void scale(const glm::vec3& scale) { m_model_mat = glm::scale(m_model_mat, scale); }
  void translate(const glm::vec3& translation) { m_model_mat = glm::translate(m_model_mat, translation); }
  const glm::mat4& model_matrix() const { return m_model_mat; }
  const glm::vec4& color() const { return m_color; }
  Mesh& mesh() { return m_mesh; }
  const Mesh& mesh() const { return m_mesh; }
protected:
  Object3D();
protected:
  Mesh m_mesh;
  glm::mat4 m_model_mat;
  glm::vec4 m_color;
};
