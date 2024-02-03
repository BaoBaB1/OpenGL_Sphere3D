#include "Pyramid.hpp"

Pyramid::Pyramid() 
{
  using list = std::initializer_list<GLuint>;
  m_mesh.vertices().reserve(5); 
  m_mesh.faces().reserve(6);
  m_mesh.vertices().emplace_back(0.f, 0.f, 0.f);
  m_mesh.vertices().emplace_back(1.f, 0.f, 0.f);
  m_mesh.vertices().emplace_back(1.f, 0.f, 1.f);
  m_mesh.vertices().emplace_back(0.f, 0.f, 1.f);
  m_mesh.vertices().emplace_back(0.5f, 1.f, 0.5f);
  m_mesh.faces().emplace_back(list{ 0, 1, 2 });
  m_mesh.faces().emplace_back(list{ 0, 2, 3 });
  m_mesh.faces().emplace_back(list{ 0, 4, 1 });
  m_mesh.faces().emplace_back(list{ 1, 4, 2 });
  m_mesh.faces().emplace_back(list{ 2, 4, 3 });
  m_mesh.faces().emplace_back(list{ 3, 4, 0 });
  m_cached_meshes[0] = m_mesh;
}
