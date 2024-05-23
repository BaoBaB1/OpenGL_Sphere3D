#pragma once

#include "Object3D.hpp"

class Cube : public Object3D
{
public:
  Cube();
  void set_texture(const std::string& filename) override;
  bool has_surface() const override { return true; }
  std::string name() const override { return "Cube"; }
};
