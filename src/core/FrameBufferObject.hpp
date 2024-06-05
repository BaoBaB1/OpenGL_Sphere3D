#pragma once

#include <glad/glad.h>
#include <optional>
#include "Texture2D.hpp"
#include "Shader.hpp"

class FrameBufferObject : public OpenGLObject
{
public:
  OnlyMovable(FrameBufferObject)
  FrameBufferObject(int w = 0, int h = 0);
  ~FrameBufferObject();
  void attach_renderbuffer(GLenum internalformat, GLenum attachment);
  void attach_texture(int w, int h, GLint internalformat, GLint format, GLint type);
  void attach_texture(Texture2D&& tex);
  void bind() const override;
  void unbind() const override;
  bool is_complete() const;
  std::optional<Texture2D>& texture() { return m_texture; }
  const std::optional<Texture2D>& texture() const { return m_texture; }
private:
  int m_width = 0;
  int m_height = 0;
  GLuint m_render_buffer_id = 0;
  std::optional<Texture2D> m_texture;
};
