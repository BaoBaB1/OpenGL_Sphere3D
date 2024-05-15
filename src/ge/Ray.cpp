#include "Ray.hpp"

bool Ray::intersect_plane(const Plane& plane, float& t) const
{
  // https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection.html
  // p0 - plane point
  // n - plane normal 
  // A vector can be calculated from any point on the plane by substracting p0 from point p.
  // Resulting vector is perpendicular to plane's normal which gives us equation:
  // 1. (p - p0) * n = 0 (dot product property of two perpendicular vectors)
  // A ray can be defined using this parametric form: 
  // 2. l0 + l * t = p, where 
  // l0 - origin
  // l - direction
  // t - is the parametric distance from the origin of the ray to the point of interest along the ray
  // insert eq2 to eq1 
  // (l0 + l * t  - p0) * n = 0 => t = ( (p0 - l0) * n) / (l * n)
  auto n_this = glm::normalize(normal);
  auto n_plane = glm::normalize(plane.normal);
  float denom = glm::dot(n_this, n_plane);
  if (denom > 1e-6)
  {
    float numerator = glm::dot((plane.point - pos), n_plane);
    t = numerator / denom; 
    return t >= 0;
  }
  return false; 
}
