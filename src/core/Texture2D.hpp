#pragma once

#include "Texture.hpp"

class Texture2D : public Texture
{
public:
  OnlyMovable(Texture2D)
  Texture2D(int w, int h, GLint internalformat, GLint format, GLint type);
  Texture2D(const std::string&);
  void resize(int w, int h, GLint internalformat, GLint format, GLint type) override;
  void resize(const std::string& filename) override;
  void bind() const override;
  void unbind() const override;
  int internal_fmt() const { return m_internal_fmt; }
  int format() const { return m_fmt; }
  int type() const { return m_type; }
private:
  int m_internal_fmt = -1;
  int m_fmt = -1;
  int m_type = -1;
};
