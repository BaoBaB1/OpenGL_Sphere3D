#include "Pyramid.hpp"

Pyramid::Pyramid() 
{
  using list = std::initializer_list<GLuint>;
  auto& mesh = m_meshes[0];
  mesh.vertices().reserve(5); 
  mesh.faces().reserve(6);
  mesh.vertices().emplace_back(0.f, 0.f, 0.f);
  mesh.vertices().emplace_back(1.f, 0.f, 0.f);
  mesh.vertices().emplace_back(1.f, 0.f, 1.f);
  mesh.vertices().emplace_back(0.f, 0.f, 1.f);
  mesh.vertices().emplace_back(0.5f, 1.f, 0.5f);
  mesh.faces().emplace_back(list{ 0, 1, 2 });
  mesh.faces().emplace_back(list{ 0, 2, 3 });
  mesh.faces().emplace_back(list{ 0, 4, 1 });
  mesh.faces().emplace_back(list{ 1, 4, 2 });
  mesh.faces().emplace_back(list{ 2, 4, 3 });
  mesh.faces().emplace_back(list{ 3, 4, 0 });
}
