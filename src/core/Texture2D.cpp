#include "Texture2D.hpp"

Texture2D::Texture2D(int w, int h, GLint internalformat, GLint format, GLint type)
{
  m_height = h;
  m_width = w;
  bind();
  glTexImage2D(GL_TEXTURE_2D, 0, internalformat, w, h, 0, format, type, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, /*GL_NEAREST*/GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, /*GL_NEAREST*/GL_LINEAR);
  unbind();
}

Texture2D::Texture2D(const std::string& filename)
{
  bind();
  stbi_set_flip_vertically_on_load(true);
  auto data = Texture::load(filename);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.get());
  // texture wrapping around x,y axes
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // filtering methods 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, /*GL_LINEAR_MIPMAP_LINEAR*/GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glGenerateMipmap(GL_TEXTURE_2D);
  m_disabled = false;
  unbind();
}

void Texture2D::bind() const
{
  glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture2D::unbind() const
{
  glBindTexture(GL_TEXTURE_2D, 0);
}
