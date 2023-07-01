#pragma once

#include <glad/glad.h>
#include <string>
#include <stdexcept>
#include <stb_image.h>

class Texture {
public:
  Texture();
  Texture(const std::string&) noexcept(false);
  ~Texture();
  void load(const std::string&);
  void disable() { m_disabled = true; }
  void enable() { m_disabled = false; }
  bool disabled() const { return m_disabled; }
  int width() const { return m_width; }
  int height() const { return m_height; }
  int nchannels() const { return m_nchannels; }
  GLuint id() const { return m_id; }
private:
  int m_width;
  int m_height;
  int m_nchannels;
  bool m_disabled;
  GLuint m_id;
};
