#pragma once

#include <array>
#include <glm/glm.hpp>
#include "./ge/Face.hpp"

class BoundingBox
{
public:
  BoundingBox();
  BoundingBox(const glm::vec3& min, const glm::vec3 max) : m_min(min), m_max(max) {}
  std::array<glm::vec3, 8> points() const;
  std::vector<GLuint> lines_indices() const;
  bool is_empty() const;
  void set_min(const glm::vec3& min) { m_min = min; }
  void set_max(const glm::vec3& max) { m_max = max; }
  glm::vec3 min() const { return m_min; }
  glm::vec3 max() const { return m_max; }
private:
  glm::vec3 m_min;
  glm::vec3 m_max; 
};