#include "Cube.hpp"

Cube::Cube() : Model(Type::CUBE) {
  m_mesh.reserve_vertices(24);
  m_mesh.reserve_faces(12);
  for (int i = 0; i < 3; ++i) {
    m_mesh.append_vertex(Vertex(0.f, 0.f, 0.f)); 
    m_mesh.append_vertex(Vertex(1.f, 0.f, 0.f)); 
    m_mesh.append_vertex(Vertex(1.f, 1.f, 0.f)); 
    m_mesh.append_vertex(Vertex(0.f, 1.f, 0.f)); 
    m_mesh.append_vertex(Vertex(0.f, 0.f, 1.f));
    m_mesh.append_vertex(Vertex(1.f, 0.f, 1.f));
    m_mesh.append_vertex(Vertex(1.f, 1.f, 1.f));
    m_mesh.append_vertex(Vertex(0.f, 1.f, 1.f));
  }
  // back
  m_mesh.append_face(Face{ 1, 0, 3 });
  m_mesh.append_face(Face{ 1, 3, 2 });
  // front
  m_mesh.append_face(Face{ 4, 5, 6 });
  m_mesh.append_face(Face{ 4, 6, 7 });
  // bottom
  m_mesh.append_face(Face{ 8, 9, 13 });
  m_mesh.append_face(Face{ 8, 13, 12 });
  // top
  m_mesh.append_face(Face{ 15, 14, 10 });
  m_mesh.append_face(Face{ 15, 10, 11 });
  // left
  m_mesh.append_face(Face{ 16, 20, 23 });
  m_mesh.append_face(Face{ 16, 23, 19 });
  // right
  m_mesh.append_face(Face{ 21, 17, 18 });
  m_mesh.append_face(Face{ 21, 18, 22 });
  // call it here to avoid vertex duplication in Model::apply_shading(),
  // but Model::apply_shading() also works fine
  calc_normals(m_mesh, IShaderable::ShadingMode::FLAT_SHADING);
  m_cached_meshes[int(IShaderable::ShadingMode::FLAT_SHADING)] = m_mesh;
}

void Cube::set_texture(const std::string& filename) {
  m_texture.load(filename);
  int cnt = 0;
  for (const auto& face : m_mesh.faces()) {
    assert(face.size == 3);
    for (int i = 0; i < face.size; ++i) 
    {
      Vertex& v = m_mesh.vertices()[face.data[i]];
      if (cnt % 2 == 0) {
        if (i == 0) {
          v.texture = glm::vec2();
        }
        else if (i == 1) {
          v.texture = glm::vec2(1.f, 0.f);
        }
        else {
          v.texture = glm::vec2(1.f, 1.f);
        }
      }
      else {
        if (i == 0) {
          v.texture = glm::vec2();
        }
        else if (i == 1) {
          v.texture = glm::vec2(1.f, 1.f);
        }
        else {
          v.texture = glm::vec2(0.f, 1.f);
        }
      }
    }
    cnt++;
  }
}
