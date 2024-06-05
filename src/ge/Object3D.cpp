#include "Object3D.hpp"

Object3D::Object3D()
{
  m_model_mat = glm::mat4(1.f);
  m_color = glm::vec4(1.f);
  m_rotation_angle = 0.f;
  m_translation = m_rotation_axis = glm::vec3();
  m_scale = glm::vec3(1.f);
  m_flags = 0;
  m_shading_mode = ShadingMode::NO_SHADING;
}

void Object3D::render(GPUBuffers* gpu_buffers, const RenderConfig& cfg)
{
  assert(gpu_buffers != nullptr);
  if (m_bbox.is_empty())
  {
    calculate_bbox();
  }
  auto& vao = gpu_buffers->vao;
  auto& vbo = gpu_buffers->vbo;
  gpu_buffers->bind_all();
  const std::vector<Vertex>& vertices = m_mesh.vertices();
  const GLuint tex_id = m_texture.has_value() ? m_texture->id() : 0;
  vbo->set_data(vertices.data(), sizeof(Vertex) * vertices.size());
  vao->link_attrib(0, 3, GL_FLOAT, sizeof(Vertex), nullptr);                         // position
  vao->link_attrib(1, 3, GL_FLOAT, sizeof(Vertex), (void*)(sizeof(GLfloat) * 3));    // normal
  vao->link_attrib(2, 4, GL_FLOAT, sizeof(Vertex), (void*)(sizeof(GLfloat) * 6));    // color
  vao->link_attrib(3, 2, GL_FLOAT, sizeof(Vertex), (void*)(sizeof(GLfloat) * 10));   // texture 
  
  glBindTexture(GL_TEXTURE_2D, tex_id);
  if (cfg.use_indices)
  {
    std::vector<GLuint> indices = m_mesh.faces_as_indices();
    auto& ebo = gpu_buffers->ebo;
    ebo->set_data(indices.data(), sizeof(GLuint) * indices.size());
    glDrawElements(cfg.mode, (GLsizei)(indices.size()), GL_UNSIGNED_INT, nullptr);
  }
  else
  {
    glDrawArrays(cfg.mode, 0, (GLsizei)vertices.size());
  }
  glBindTexture(GL_TEXTURE_2D, 0);

  if (is_normals_visible())
  {
    // normals without shading
    GLint current_shader_id = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_shader_id);
    assert(current_shader_id != 0);
    glUniform1i(glGetUniformLocation(current_shader_id, "applyShading"), false);
    std::vector<Vertex> normals = std::move(normals_as_lines());
    vbo->set_data(normals.data(), sizeof(Vertex) * normals.size());
    vao->link_attrib(0, 3, GL_FLOAT, sizeof(Vertex), nullptr);                      // position
    vao->link_attrib(2, 4, GL_FLOAT, sizeof(Vertex), (void*)(sizeof(GLfloat) * 6)); // color
    glDrawArrays(GL_LINES, 0, (GLsizei)normals.size());
  }
  if (is_bbox_visible())
  {
    m_bbox.render(gpu_buffers);
  }
  gpu_buffers->unbind_all();
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
  m_texture = Texture2D(filename);
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

BoundingBox Object3D::calculate_bbox()
{
  glm::vec3 pos_min, pos_max;
  pos_min = pos_max = m_mesh.vertices()[0].position;
  for (size_t i = 1; i < m_mesh.vertices().size(); i++)
  {
    const glm::vec3 pos = m_mesh.vertices()[i].position;
    if (pos.x > pos_max.x)
      pos_max.x = pos.x;
    if (pos.x < pos_min.x)
      pos_min.x = pos.x;
    if (pos.y > pos_max.y)
      pos_max.y = pos.y;
    if (pos.y < pos_min.y)
      pos_min.y = pos.y;
    if (pos.z > pos_max.z)
      pos_max.z = pos.z;
    if (pos.z < pos_min.z)
      pos_min.z = pos.z;
  }
  m_bbox.set_min(pos_min);
  m_bbox.set_max(pos_max);
  return m_bbox;
}

void Object3D::set_color(const glm::vec4& color)
{
  m_color = color;
  for (auto& vertex : m_mesh.vertices())
  {
    vertex.color = color;
  }
  for (auto& m : m_cached_meshes)
  {
    for (auto& vertex : m.vertices())
    {
      vertex.color = color;
    }
  }
}

glm::vec3 Object3D::center()
{
  assert(m_mesh.vertices().size() > 0);
  float min_x, max_x, min_y, max_y, min_z, max_z;
  min_x = max_x = m_mesh.vertices()[0].position.x;
  min_y = max_y = m_mesh.vertices()[0].position.y;
  min_z = max_z = m_mesh.vertices()[0].position.z;
  for (size_t i = 1; i < m_mesh.vertices().size(); i++)
  {
    glm::vec3 pos = m_mesh.vertices()[i].position;
    if (pos.x > max_x)
      max_x = pos.x;
    else if (pos.x < min_x)
      min_x = pos.x;
    if (pos.y > max_y)
      max_y = pos.y;
    else if (pos.y < min_y)
      min_y = pos.y;
    if (pos.z > max_z)
      max_z = pos.z;
    else if (pos.z < min_z)
      min_z = pos.z;
  }
  return glm::vec3((max_x + min_x) / 2.f, (max_y + min_y) / 2.f, (max_z + min_z) / 2.f);
}

void Object3D::render(GPUBuffers* gpu_buffers)
{
  render(gpu_buffers, RenderConfig::default());
}

void Object3D::apply_shading(Object3D::ShadingMode mode)
{
  if (mode != m_shading_mode)
  {
    int current_mode_idx = static_cast<int>(m_shading_mode);
    // if mesh with current shading mode is not cached yet
    if (m_cached_meshes[current_mode_idx].vertices().size() == 0)
    {
      m_cached_meshes[current_mode_idx] = m_mesh;
    }
    m_shading_mode = mode;

    if (mode == Object3D::ShadingMode::NO_SHADING)
    {
      for (Vertex& v : m_mesh.vertices())
        v.normal = glm::vec3(0.f);
    }
    else
    {
      int new_mode_idx = static_cast<int>(mode);
      // not cached data yet
      if (m_cached_meshes[new_mode_idx].vertices().size() == 0)
      {
        m_vertex_finder.m_map_vert.clear();
        // duplicate each vertex of current mesh for flat shading so each face has its own normal
        if (mode == Object3D::ShadingMode::FLAT_SHADING)
        {
          std::vector<GLuint> indices(3);
          std::vector<Face>& faces = m_mesh.faces(), new_faces;
          for (const auto& face : faces)
          {
            indices.resize(face.size);
            for (int i = 0; i < face.size; ++i)
            {
              Vertex vert = m_mesh.vertices()[face.data[i]];
              VertexFinder::iter iter = m_vertex_finder.find_vertex(vert);
              // this vertex already present
              if (iter != m_vertex_finder.end())
              {
                // add copy
                indices[i] = static_cast<GLuint>(m_mesh.append_vertex(vert));
              }
              else
              {
                indices[i] = face.data[i];
                m_vertex_finder.add_vertex(vert, indices[i]);
              }
            }
            new_faces.push_back(indices);
          }
          faces = std::move(new_faces);
        }
        // for smooth shading we have to make sure that every vertex is unique as well as it's normal.
        // fragment color will be interpolated between triangle's vertex normals 
        else if (mode == Object3D::ShadingMode::SMOOTH_SHADING)
        {
          std::vector<GLuint> indices(3);
          std::vector<Vertex>& vertices = m_mesh.vertices(), unique_vertices;
          std::vector<Face>& faces = m_mesh.faces();
          assert(faces.size() > 0);
          for (auto& face : faces)
          {
            indices.resize(face.size);
            for (int i = 0; i < face.size; ++i)
            {
              Vertex vert = vertices[face.data[i]];
              VertexFinder::iter iter = m_vertex_finder.find_vertex(vert);
              if (iter != m_vertex_finder.end())
              {
                face.data[i] = iter->second;
              }
              else
              {
                unique_vertices.push_back(vert);
                face.data[i] = unique_vertices.size() - 1;
                m_vertex_finder.add_vertex(vert, face.data[i]);
              }
            }
          }
          vertices = std::move(unique_vertices);
        }
        calc_normals(m_mesh, mode);
      }
      else
      {
        m_mesh = m_cached_meshes[new_mode_idx];
      }
    }
  }
}

void Object3D::calc_normals(Mesh& mesh, ShadingMode mode)
{
  if (mode == ShadingMode::NO_SHADING)
    return;
  std::vector<Vertex>& vertices = mesh.vertices();
  const std::vector<Face>& faces = mesh.faces();
  for (auto& vert : vertices)
  {
    vert.normal = glm::vec3(0.f);
  }
  for (auto& face : faces)
  {
    assert(face.size == 3);
    GLuint ind = face.data[0], ind2 = face.data[1], ind3 = face.data[2];
    glm::vec3 a = vertices[ind2].position - vertices[ind].position;
    glm::vec3 b = vertices[ind3].position - vertices[ind].position;
    glm::vec3 normal = glm::cross(a, b);
    // TODO: some triangles may be in CW order while other in CCW and it affects on normal.
    // so here would be nice somehow check if normal is pointing inside or outside.
    // providing such functionality will avoid defining all faces in CW or CCW order
    // as their normals will always point outside despite their order given in constructor
    // AND/OR make all faces in same winding if there are some in different
    if (mode == ShadingMode::SMOOTH_SHADING)
    {
      vertices[ind].normal += normal;
      vertices[ind2].normal += normal;
      vertices[ind3].normal += normal;
    }
    else if (ShadingMode::FLAT_SHADING)
    {
      vertices[ind].normal = normal;
      vertices[ind2].normal = normal;
      vertices[ind3].normal = normal;
    }
  }
  for (Vertex& v : vertices)
    if (v.normal != glm::vec3())
      v.normal = glm::normalize(v.normal);
}

