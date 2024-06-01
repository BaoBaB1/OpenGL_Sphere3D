#include "Cubemap.hpp"

Cubemap::Cubemap(const std::array<std::string, 6>& textures)
{
  bind();
  stbi_set_flip_vertically_on_load(false);
  for (int i = 0; i < (int)textures.size(); i++)
  {
    auto data = Texture::load(textures[i]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.get());
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  unbind();
}

void Cubemap::bind() const
{
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
}

void Cubemap::unbind() const
{
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
