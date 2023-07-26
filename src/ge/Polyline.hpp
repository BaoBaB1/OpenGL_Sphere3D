#pragma once

#include <vector>
#include "./ge/Object3D.hpp"
#include "./core/GPUBuffers.hpp"
#include "./core/Shader.hpp"
#include "./ge/Vertex.hpp"

class Polyline : public Object3D {
public:
  Polyline() = default;
  std::string name() const override { return "Polyline"; }
  bool has_surface() const override { return false; }
  void render(GPUBuffers*, Shader*) override;
  void add(const Vertex& point);
};
