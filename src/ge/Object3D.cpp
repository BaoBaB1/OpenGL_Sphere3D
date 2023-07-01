#include "Object3D.hpp"

Object3D::Object3D() {
  m_model_mat = glm::mat4(1.f);
  m_color = glm::vec4(1.f);
}

void Object3D::set_color(const glm::vec4& color) {
  m_color = color;
  for (auto& vertex : m_mesh.vertices()) {
    vertex.color = color;
  }
}
