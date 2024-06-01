#pragma once

#include "Texture.hpp"
#include <array>

class Cubemap : public Texture
{
public:
  Cubemap() = default;
  OnlyMovable(Cubemap)
  Cubemap(const std::array<std::string, 6>& textures);
  void bind() const override;
  void unbind() const override;
};
