#pragma once

#include <glad/glad.h>
#include <memory>
#include "Texture2D.hpp"
#include "Shader.hpp"

class FrameBufferObject : public OpenGLObject
{
public:
  OnlyMovable(FrameBufferObject)
  FrameBufferObject(int w, int h);
  ~FrameBufferObject();
  template<typename... Args>
  void create_shader(Args&&... args);
  void attach_renderbuffer(GLenum internalformat, GLenum attachment);
  void attach_texture(int w, int h, GLint internalformat, GLint format, GLint type);
  void bind() const override;
  void unbind() const override;
  bool is_complete() const;
  Texture2D& texture() { return *m_texture; }
  Shader& shader() { return *m_shader; }
private:
  int m_width = 0;
  int m_height = 0;
  GLuint m_render_buffer_id = 0;
  std::unique_ptr<Texture2D> m_texture;
  std::unique_ptr<Shader> m_shader;
};

template<typename... Args>
void FrameBufferObject::create_shader(Args&&... args)
{
  m_shader = std::make_unique<Shader>(std::forward<Args>(args)...);
}
