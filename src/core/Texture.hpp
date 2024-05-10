#pragma once

#include <glad/glad.h>
#include <string>
#include <stdexcept>
#include <stb_image.h>

class Texture {
public:
  Texture();
  Texture(int w, int h, GLint internalformat, GLint format, GLint type);
  Texture(const std::string&) noexcept(false);
  ~Texture();
  void bind();
  void unbind();
  void load(const std::string&);
  void disable() { m_disabled = true; }
  void enable() { m_disabled = false; }
  bool disabled() const { return m_disabled; }
  int width() const { return m_width; }
  int height() const { return m_height; }
  int nchannels() const { return m_nchannels; }
  GLuint id() const { return m_id; }
private:
  int m_width = 0;
  int m_height = 0;
  int m_nchannels = 0;
  bool m_disabled = true;
  GLuint m_id = 0;
};
