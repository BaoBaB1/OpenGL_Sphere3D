#pragma once

#include "Model.hpp"

class Pyramid : public Model {
public:
  Pyramid();
  std::string name() const override { return "Pyramid"; }
};
