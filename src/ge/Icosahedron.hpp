#pragma once

#include "Object3D.hpp"

class Icosahedron : public Object3D
{
public:
  Icosahedron();
  std::string name() const override { return "Icosahedron"; }
  bool has_surface() const override { return true; }
  void subdivide_triangles(int subdivision_depth);
  void project_points_on_sphere();
private:
  void subdivide_triangles(int subdivision_level, const Vertex& a, const Vertex& b, const Vertex& c);
  void allocate_memory_before_subdivision(int subdivision_depth, int face_count);
};
