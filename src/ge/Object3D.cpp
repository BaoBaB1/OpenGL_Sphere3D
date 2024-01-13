#include "Object3D.hpp"

Object3D::Object3D() 
{
  m_model_mat = glm::mat4(1.f);
  m_color = glm::vec4(1.f);
  m_rotation_angle = 0.f;
  m_translation = m_rotation_axis = glm::vec3();
  m_scale = glm::vec3(1.f);
  m_flags = 0;
}

void Object3D::render(GPUBuffers* gpu_buffers, Shader* shader, RenderConfig* cfg)
{
  assert(gpu_buffers != nullptr && shader != nullptr && cfg != nullptr);
  shader->set_bool("applyShading", cfg->apply_shading);
  if (is_rotating())
    rotate(m_rotation_angle, m_rotation_axis);
  VertexArrayObject* vao = gpu_buffers->vao;
  VertexBufferObject* vbo = gpu_buffers->vbo;
  ElementBufferObject* ebo = gpu_buffers->ebo;
  gpu_buffers->bind_all();

  std::vector<Vertex>& vertices = m_mesh.vertices();
  std::vector<GLuint> indices;
  if (cfg->use_indices)
  {
    indices = std::move(m_mesh.faces_as_indices());
    ebo->set_data(indices.data(), sizeof(GLuint) * indices.size());
  }
  vbo->set_data(vertices.data(), sizeof(Vertex) * vertices.size());
  vao->link_attrib(0, 3, GL_FLOAT, sizeof(Vertex), nullptr);                         // position
  vao->link_attrib(1, 3, GL_FLOAT, sizeof(Vertex), (void*)(sizeof(GLfloat) * 3));    // normal
  vao->link_attrib(2, 4, GL_FLOAT, sizeof(Vertex), (void*)(sizeof(GLfloat) * 6));    // color
  vao->link_attrib(3, 2, GL_FLOAT, sizeof(Vertex), (void*)(sizeof(GLfloat) * 10));   // texture coords
  glBindTexture(GL_TEXTURE_2D, m_texture.id());
  if (cfg->use_indices)
    glDrawElements(cfg->mode, indices.size(), GL_UNSIGNED_INT, nullptr);
  else
    glDrawArrays(cfg->mode, 0, vertices.size());
  glBindTexture(GL_TEXTURE_2D, 0);
  if (is_normals_visible())
  {
    // normals without shading
    shader->set_bool("applyShading", false);
    std::vector<Vertex> normals = std::move(normals_as_lines());
    vbo->set_data(normals.data(), sizeof(Vertex) * normals.size());
    vao->link_attrib(0, 3, GL_FLOAT, sizeof(Vertex), nullptr);                      // position
    vao->link_attrib(2, 4, GL_FLOAT, sizeof(Vertex), (void*)(sizeof(GLfloat) * 6)); // color
    glDrawArrays(GL_LINES, 0, normals.size());
  }
  gpu_buffers->unbind_all();
}

void Object3D::set_color(const glm::vec4& color)
{
  m_color = color;
  for (auto& vertex : m_mesh.vertices())
    vertex.color = color;
}

void Object3D::rotate(float angle, const glm::vec3& axis) 
{
  if (axis == glm::vec3())
    return;
  m_rotation_angle = angle; 
  m_rotation_axis = axis;
  if (angle != 0.f)
    m_model_mat = glm::rotate(m_model_mat, glm::radians(angle), axis);
}

void Object3D::scale(const glm::vec3& scale)
{
  // get rid of current scale factor (https://gamedev.stackexchange.com/questions/119702/fastest-way-to-neutralize-scale-in-the-transform-matrix)
  for (int i = 0; i < 3; i++)
    m_model_mat[i] = glm::normalize(m_model_mat[i]);
  m_scale = scale;
  m_model_mat = glm::scale(m_model_mat, scale);
}

void Object3D::translate(const glm::vec3& translation)
{
  m_model_mat = glm::translate(m_model_mat, translation);
  m_translation = glm::vec3(m_model_mat[3]);
}

void Object3D::set_texture(const std::string& filename)
{
  m_texture.load(filename);
  for (const auto& face : m_mesh.faces())
  {
    assert(face.size == 3);
    for (int i = 0; i < face.size; ++i)
    {
      // temporary basic implementation. doesn't work well 
      Vertex& v = m_mesh.vertices()[face.data[i]];
      v.texture = glm::normalize(v.position);
    }
  }
}

std::vector<Vertex> Object3D::normals_as_lines()
{
  std::vector<Vertex> normals(m_mesh.vertices().size() * 2);
  float len_scaler = 3.f;
  size_t index = 0;
  for (const auto& vertex : m_mesh.vertices())
  {
    normals[index].position = vertex.position;
    normals[index + 1].position = vertex.position + vertex.normal / len_scaler;
    normals[index].color = normals[index + 1].color = glm::vec4(0.f, 1.f, 1.f, 1.f);
    index += 2;
  }
  return normals;
}
