#pragma once

#include <glm/glm.hpp>
#include "Plane.hpp"

struct Ray
{
  bool intersect_plane(const Plane& plane, float& t) const;
  glm::vec3 pos; 
  glm::vec3 normal;
};
