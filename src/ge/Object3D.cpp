#include "Object3D.hpp"

Object3D::Object3D()
{
  m_meshes.resize(1);
}

void Object3D::render(GPUBuffers* gpu_buffers, const RenderConfig& cfg)
{
  assert(gpu_buffers != nullptr);
  gpu_buffers->bind_all();
  if (m_bbox.is_empty())
  {
    m_bbox = calculate_bbox();
  }

  for (const auto& mesh : m_meshes)
  {
    auto& vao = gpu_buffers->vao;
    auto& vbo = gpu_buffers->vbo;
    const std::vector<Vertex>& vertices = mesh.vertices();
    const auto& texture = mesh.texture();
    const GLuint tex_id = texture ? texture->id() : 0;
    vbo->set_data(vertices.data(), sizeof(Vertex) * vertices.size());
    vao->link_attrib(0, 3, GL_FLOAT, sizeof(Vertex), nullptr);                         // position
    vao->link_attrib(1, 3, GL_FLOAT, sizeof(Vertex), (void*)(sizeof(GLfloat) * 3));    // normal
    vao->link_attrib(2, 4, GL_FLOAT, sizeof(Vertex), (void*)(sizeof(GLfloat) * 6));    // color
    vao->link_attrib(3, 2, GL_FLOAT, sizeof(Vertex), (void*)(sizeof(GLfloat) * 10));   // texture 

    glBindTexture(GL_TEXTURE_2D, tex_id);
    if (cfg.use_indices)
    {
      std::vector<GLuint> indices = mesh.faces_as_indices();
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
      render_lines_and_reset_shader(&Object3D::render_normals, this, gpu_buffers, mesh);
    }
  }

  if (is_bbox_visible())
  {
    render_lines_and_reset_shader(&BoundingBox::render, &m_bbox, gpu_buffers);
  }

  // normal lines are recalculated in Object3D::render_normals
  if (is_normals_visible() && get_flag(RESET_CACHED_NORMALS))
  {
    set_flag(RESET_CACHED_NORMALS, false);
  }

  gpu_buffers->unbind_all();
}

void Object3D::rotate(float angle, const glm::vec3& axis)
{
  if (axis == glm::vec3())
    return;
  constexpr float rotation_speed = 10.f;
  set_flag(RESET_CACHED_NORMALS, true);
  m_model_mat = glm::rotate(m_model_mat, glm::radians(angle * m_delta_time * rotation_speed), glm::normalize(axis));
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
  for (auto& mesh : m_meshes)
  {
    mesh.texture() = std::make_shared<Texture2D>(filename);
    for (const auto& face : mesh.faces())
    {
      assert(face.size == 3);
      for (int i = 0; i < face.size; ++i)
      {
        // temporary basic implementation. doesn't work well 
        Vertex& v = mesh.vertices()[face.data[i]];
        v.texture = glm::normalize(v.position);
      }
    }
  }
}

std::vector<Vertex> Object3D::normals_as_lines(const Mesh& mesh)
{
  // Too slow to call every frame if object has a lot of vertices
  std::vector<Vertex> normals(mesh.vertices().size() * 2);
  constexpr float len_scaler = 3.f;
  size_t index = 0;
  for (const auto& vertex : mesh.vertices())
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
  glm::vec3 pos_min(0.f), pos_max(0.f);
  BoundingBox bbox;
  for (const auto& mesh : m_meshes)
  {
    for (size_t i = 0; i < mesh.vertices().size(); i++)
    {
      const glm::vec3 pos = mesh.vertices()[i].position;
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
  }
  bbox.set_min(pos_min);
  bbox.set_max(pos_max);
  return bbox;
}

bool Object3D::has_active_texture() const
{
  return std::find_if(m_meshes.begin(), m_meshes.end(), 
    [](const Mesh& mesh) 
    { 
      return mesh.texture() && !mesh.texture()->disabled(); 
    }) != m_meshes.end();
}

void Object3D::set_color(const glm::vec4& color)
{
  m_color = color;
  auto apply_color = [](std::vector<Mesh>& meshes, const glm::vec4& color) 
    {
      for (auto& mesh : meshes)
      {
        for (auto& vertex : mesh.vertices())
        {
          vertex.color = color;
        }
      }
    };

  // set color of current mesh
  apply_color(m_meshes, color);

  // set color of cached meshes
  for (auto& cached_meshes : m_cached_meshes)
  {
    apply_color(cached_meshes.second, color);
  }
}

glm::vec3 Object3D::center()
{
  assert(m_meshes.size() > 0);
  float min_x, max_x, min_y, max_y, min_z, max_z;
  min_x = max_x = min_y = max_y = min_z = max_z = 0.f;
  for (auto& mesh : m_meshes)
  {
    for (size_t i = 0; i < mesh.vertices().size(); i++)
    {
      glm::vec3 pos = mesh.vertices()[i].position;
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
  }
  return glm::vec3((max_x + min_x) * 0.5f, (max_y + min_y) * 0.5f, (max_z + min_z) * 0.5f);
}

void Object3D::render(GPUBuffers* gpu_buffers)
{
  render(gpu_buffers, RenderConfig::default());
}

void Object3D::apply_shading(Object3D::ShadingMode mode)
{
  if (mode != m_shading_mode)
  {
    set_flag(RESET_CACHED_NORMALS, true);
    // if meshes with current shading mode are not cached yet
    if (m_cached_meshes.count(m_shading_mode) == 0)
    {
      m_cached_meshes[m_shading_mode] = m_meshes;
    }
    m_shading_mode = mode;

    // if meshes with new shading mode have already been cached
    if (m_cached_meshes.count(mode) != 0)
    {
      m_meshes = m_cached_meshes[mode];
      return;
    }

    // no shading, all normals == 0
    if (mode == Object3D::ShadingMode::NO_SHADING)
    {
      for (auto& mesh : m_meshes)
      {
        for (Vertex& v : mesh.vertices())
        {
          v.normal = glm::vec3(0.f);
        }
      }
    }

    // duplicate each vertex of current mesh for flat shading so each face has its own normal
    else if (mode == Object3D::ShadingMode::FLAT_SHADING)
    {
      for (auto& mesh : m_meshes)
      {
        m_vertex_finder.m_map_vert.clear();
        std::vector<GLuint> indices(3);
        std::vector<Face>& faces = mesh.faces(), new_faces;
        assert(faces.size() > 0);
        for (const auto& face : faces)
        {
          assert(face.size == 3);
          for (int i = 0; i < face.size; ++i)
          {
            Vertex vert = mesh.vertices()[face.data[i]];
            VertexFinder::iter iter = m_vertex_finder.find_vertex(vert);
            // this vertex already present
            if (iter != m_vertex_finder.end())
            {
              // add copy
              indices[i] = static_cast<GLuint>(mesh.append_vertex(vert));
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
        calc_normals(mesh, mode);
      }
    }

    // for smooth shading we have to make sure that every vertex is unique as well as it's normal.
    // fragment color will be interpolated between triangle's vertex normals 
    else if (mode == Object3D::ShadingMode::SMOOTH_SHADING)
    {
      for (auto& mesh : m_meshes)
      {
        m_vertex_finder.m_map_vert.clear();
        std::vector<GLuint> indices(3);
        std::vector<Vertex>& vertices = mesh.vertices(), unique_vertices;
        std::vector<Face>& faces = mesh.faces();
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
        calc_normals(mesh, mode);
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

void Object3D::render_normals(GPUBuffers* buffers, const Mesh& mesh)
{
  auto& vbo = buffers->vbo;
  auto& vao = buffers->vao;
  std::vector<Vertex> normals;
  if (get_flag(RESET_CACHED_NORMALS))
  {
    normals = normals_as_lines(mesh);
    const_cast<Mesh&>(mesh).m_cached_normals = normals;
  }
  else
  {
    normals = mesh.m_cached_normals;
  }
  vbo->set_data(normals.data(), sizeof(Vertex) * normals.size());
  vao->link_attrib(0, 3, GL_FLOAT, sizeof(Vertex), nullptr);                      // position
  vao->link_attrib(1, 4, GL_FLOAT, sizeof(Vertex), (void*)(sizeof(GLfloat) * 6)); // color
  glDrawArrays(GL_LINES, 0, (GLsizei)normals.size());
}

