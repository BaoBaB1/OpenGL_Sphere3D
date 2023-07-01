#pragma once

#include "Model.hpp"

class Cube : public Model {
public:
  Cube();
  void set_texture(const std::string& filename) override;
};
