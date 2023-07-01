#include "Model.hpp"

Model::Model(Type type) {
  m_flags = 0;
  m_type = type;
  m_shading_mode = IShaderable::ShadingMode::NO_SHADING;
}

void Model::set_color(const glm::vec4& color) {
  Object3D::set_color(color);
  for (auto& m : m_cached_meshes) {
    for (auto& vertex : m.vertices()) {
      vertex.color = color;
    }
  }
}

void Model::set_texture(const std::string& filename) {
  m_texture.load(filename);
  for (const auto& face : m_mesh.faces()) {
    assert(face.size == 3);
    for (int i = 0; i < face.size; ++i) {
      // temporary basic implementation. doesn't work well 
      Vertex& v = m_mesh.vertices()[face.data[i]];
      v.texture = glm::normalize(v.position);
    }
  }
}

glm::vec3 Model::center() {
  assert(m_mesh.vertices().size() > 0);
  float min_x, max_x, min_y, max_y, min_z, max_z;
  min_x = max_x = m_mesh.vertices()[0].position.x;
  min_y = max_y = m_mesh.vertices()[0].position.y;
  min_z = max_z = m_mesh.vertices()[0].position.z;
  for (size_t i = 1; i < m_mesh.vertices().size(); i++) {
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

void Model::render(GPUBuffers* gpu_buffers, Shader* shader) {
  assert(gpu_buffers != nullptr && shader != nullptr);
  if (this->is_rotating()) {
    rotate(1.f, glm::vec3(1.f, 1.f, 0.f));
  }
  if (this->is_light_source()) {
    // center in world space
    shader->set_vec3("lightPos", this->center() + glm::vec3(m_model_mat[3]));
    shader->set_vec3("lightColor", glm::vec3(1.f));
    float x = std::sin(glfwGetTime());
    float y = std::cos(glfwGetTime());
    translate(glm::vec3(-x / 20, -y / 20, 0.f));
  }
  shader->set_bool("applyShading", m_shading_mode != IShaderable::ShadingMode::NO_SHADING && !this->is_light_source());
  shader->set_bool("applyTexture", !m_texture.disabled());
  VertexArrayObject* vao = gpu_buffers->vao;
  VertexBufferObject* vbo = gpu_buffers->vbo;
  ElementBufferObject* ebo = gpu_buffers->ebo;
  gpu_buffers->bind_all();
  std::vector<Vertex>& vertices = m_mesh.vertices();
  std::vector<GLuint> indices(std::move(m_mesh.faces_as_indices()));
  vbo->set_data(vertices.data(), sizeof(Vertex) * vertices.size());
  ebo->set_data(indices.data(), sizeof(GLuint) * indices.size());
  vao->link_attrib(0, 3, GL_FLOAT, sizeof(Vertex), nullptr);              // position
  vao->link_attrib(1, 3, GL_FLOAT, sizeof(Vertex), (void*)(sizeof(GLfloat) * 3));    // normal
  vao->link_attrib(2, 4, GL_FLOAT, sizeof(Vertex), (void*)(sizeof(GLfloat) * 6));    // color
  vao->link_attrib(3, 2, GL_FLOAT, sizeof(Vertex), (void*)(sizeof(GLfloat) * 10));   // texture coords
  glBindTexture(GL_TEXTURE_2D, m_texture.id());
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
  glBindTexture(GL_TEXTURE_2D, 0);
  if (this->is_normals_visible()) {
    shader->set_bool("applyShading", false);
    std::vector<Vertex> normals = std::move(normals_as_lines());
    vbo->set_data(normals.data(), sizeof(Vertex) * normals.size());
    vao->link_attrib(0, 3, GL_FLOAT, sizeof(Vertex), nullptr);            // position
    vao->link_attrib(2, 4, GL_FLOAT, sizeof(Vertex), (void*)(sizeof(GLfloat) * 6)); // color
    glDrawArrays(GL_LINES, 0, normals.size());
  }
  gpu_buffers->unbind_all();
}

void Model::apply_shading(IShaderable::ShadingMode mode) {
  if (mode != m_shading_mode)
  {
    m_shading_mode = mode;
    if (mode == IShaderable::ShadingMode::NO_SHADING) {
      for (Vertex& v : m_mesh.vertices())
        v.normal = glm::vec3(0.f);
    }
    else
    {
      int index = static_cast<int>(mode);
      // not cached data yet
      if (m_cached_meshes[index].vertices().size() == 0)
      {
        m_vertex_finder.m_map_vert.clear();
        // duplicate each vertex of current mesh for flat shading so each face has its own normal
        if (mode == IShaderable::ShadingMode::FLAT_SHADING)
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
        else if (mode == IShaderable::ShadingMode::SMOOTH_SHADING)
        {
          std::vector<GLuint> indices(3);
          std::vector<Vertex>& vertices = m_mesh.vertices(), unique_vertices;
          std::vector<Face>& faces = m_mesh.faces();
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
        m_cached_meshes[index] = m_mesh;
      }
      else {
        m_mesh = m_cached_meshes[index];
      }
    }
  }
}

void Model::calc_normals(Mesh& mesh, IShaderable::ShadingMode mode) {
  if (mode == IShaderable::NO_SHADING)
    return;
  std::vector<Vertex>& vertices = mesh.vertices();
  const std::vector<Face>& faces = mesh.faces();
  for (auto& vert : vertices) {
    vert.normal = glm::vec3(0.f);
  }
  for (auto& face : faces) {
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
    if (mode == IShaderable::ShadingMode::SMOOTH_SHADING) {
      vertices[ind].normal += normal;
      vertices[ind2].normal += normal;
      vertices[ind3].normal += normal;
    }
    else if (IShaderable::ShadingMode::FLAT_SHADING) {
      vertices[ind].normal = normal;
      vertices[ind2].normal = normal;
      vertices[ind3].normal = normal;
    }
  }
  for (Vertex& v : vertices) {
    if (v.normal != glm::vec3()) {
      v.normal = glm::normalize(v.normal);
    }
  }
}

std::vector<Vertex> Model::normals_as_lines() {
  std::vector<Vertex> normals(m_mesh.vertices().size() * 2);
  float len_scaler = 3.f;
  size_t index = 0;
  for (const auto& vertex : m_mesh.vertices()) {
    normals[index].position = vertex.position;
    normals[index + 1].position = vertex.position + vertex.normal / len_scaler;
    normals[index].color = normals[index + 1].color = glm::vec4(0.f, 1.f, 1.f, 1.f);
    index += 2; 
  }
  return normals;
}
