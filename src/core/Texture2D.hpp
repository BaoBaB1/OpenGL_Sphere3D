#pragma once

#include "Texture.hpp"

class Texture2D : public Texture
{
public:
  OnlyMovable(Texture2D)
  Texture2D(int w, int h, GLint internalformat, GLint format, GLint type);
  Texture2D(const std::string&);
  void bind() const override;
  void unbind() const override;
};
