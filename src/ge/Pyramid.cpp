#include "Pyramid.hpp"

Pyramid::Pyramid() : Model(Model::Type::PYRAMID) {
	m_mesh.vertices().reserve(5); 
	m_mesh.faces().reserve(6);
	m_mesh.vertices().emplace_back(Vertex(0.f, 0.f, 0.f));
	m_mesh.vertices().emplace_back(Vertex(1.f, 0.f, 0.f));
	m_mesh.vertices().emplace_back(Vertex(1.f, 0.f, 1.f));
	m_mesh.vertices().emplace_back(Vertex(0.f, 0.f, 1.f));
	m_mesh.vertices().emplace_back(Vertex(0.5f, 1.f, 0.5f));
	m_mesh.faces().emplace_back(Face{ 0, 1, 2 });
	m_mesh.faces().emplace_back(Face{ 0, 2, 3 });
	m_mesh.faces().emplace_back(Face{ 0, 4, 1 });
	m_mesh.faces().emplace_back(Face{ 1, 4, 2 });
	m_mesh.faces().emplace_back(Face{ 2, 4, 3 });
	m_mesh.faces().emplace_back(Face{ 3, 4, 0 });
	m_cached_meshes[0] = m_mesh;
}
