#include "utils/Constants.hpp"
#include "BoundingBox.hpp"
#include "Vertex.hpp"

static constexpr float g_fmin = OpenGLEngineUtils::limits::fmin;
static constexpr float g_fmax = OpenGLEngineUtils::limits::fmax;

BoundingBox::BoundingBox()
{
  m_min = glm::vec3(g_fmin, g_fmin, g_fmin);
  m_max = glm::vec3(g_fmax, g_fmax, g_fmax);
}

bool BoundingBox::is_empty() const
{
  return m_min == glm::vec3(g_fmin, g_fmin, g_fmin) && m_max == glm::vec3(g_fmax, g_fmax, g_fmax);
}

void BoundingBox::render(GPUBuffers* buffers, Shader* shader)
{
  // BoundingBox::render must be called only inside Object3D::render, 
  // thus all buffers must be already bound
  VertexArrayObject* vao = buffers->vao;
  VertexBufferObject* vbo = buffers->vbo;
  shader->set_bool("applyShading", false);
  std::array<glm::vec3, 8> bbox_points = points();
  std::array<Vertex, 8> converted;
  for (size_t i = 0; i < 8; i++)
  {
    converted[i] = Vertex(bbox_points[i]);
    converted[i].color = glm::vec4(0.f, 1.f, 0.f, 1.f);
  }
  auto indices = lines_indices();
  ElementBufferObject* ebo = buffers->ebo;
  vbo->set_data(converted.data(), sizeof(Vertex) * 8);
  vao->link_attrib(0, 3, GL_FLOAT, sizeof(Vertex), nullptr);                      // position
  vao->link_attrib(2, 4, GL_FLOAT, sizeof(Vertex), (void*)(sizeof(GLfloat) * 6)); // color
  ebo->set_data(indices.data(), sizeof(GLuint) * indices.size());
  glDrawElements(GL_LINES, (GLsizei)indices.size(), GL_UNSIGNED_INT, nullptr);
}

bool BoundingBox::contains(const glm::vec3& point) const
{
  return (point.x >= m_min.x && point.x <= m_max.x) &&
    (point.y >= m_min.y && point.y <= m_max.y) &&
    (point.z >= m_min.z && point.z <= m_max.z);
}

std::array<glm::vec3, 8> BoundingBox::points() const
{
  std::array<glm::vec3, 8> points;
  // llc - left lower corner, rtc - right top corner ...
  // front quad (llc -> rlc -> rtc -> ltc) 
  points[0] = m_min;
  points[1] = glm::vec3(m_max.x, m_min.y, m_min.z);
  points[2] = glm::vec3(m_max.x, m_max.y, m_min.z);
  points[3] = glm::vec3(m_min.x, m_max.y, m_min.z);
  // back quad (llc -> rlc -> rtc -> ltc)
  points[4] = glm::vec3(m_min.x, m_min.y, m_max.z);
  points[5] = glm::vec3(m_max.x, m_min.y, m_max.z);
  points[6] = m_max;
  points[7] = glm::vec3(m_min.x, m_max.y, m_max.z);
  return points;
}

std::vector<GLuint> BoundingBox::lines_indices() const
{
  std::vector<GLuint> v = {
    0, 1, 1, 2, 2, 3, 3, 0, // front
    4, 5, 5, 6, 6, 7, 7, 4, // back
    0, 4, 3, 7, 1, 5, 2, 6
  };
  return v;
}
