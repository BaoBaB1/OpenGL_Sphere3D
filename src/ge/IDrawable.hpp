#pragma once

#include <string>
#include "./core/GPUBuffers.hpp"
#include "./core/Shader.hpp"

class IDrawable 
{
public:
  virtual void render(GPUBuffers*, Shader*) = 0;
  virtual bool has_surface() const = 0;
  virtual std::string name() const = 0;
  virtual ~IDrawable() = default;
};
