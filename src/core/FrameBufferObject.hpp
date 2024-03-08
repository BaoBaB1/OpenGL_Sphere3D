#pragma once

#include <glad/glad.h>
#include <memory>
#include "Texture.hpp"
#include "Shader.hpp"

class FrameBufferObject
{
public:
  FrameBufferObject(int w, int h);
  ~FrameBufferObject();
  template<typename... Args>
  void create_shader(Args&&... args);
  void create_texture(int w, int h);
  void create_depth_buffer();
  void attach_depth_buffer();
  void attach_current_texture();
  void bind() const;
  void unbind() const;
  bool is_complete() const;
  GLuint id() const { return m_id; }
  GLuint depth_buffer_id() const { return m_depth_buffer_id; }
  Texture& texture() { return *m_texture; }
  Shader& shader() { return *m_shader; }
private:
  int m_width = 0;
  int m_height = 0;
  GLuint m_id = 0;
  GLuint m_depth_buffer_id = 0;
  std::unique_ptr<Texture> m_texture;
  std::unique_ptr<Shader> m_shader;
};

template<typename... Args>
void FrameBufferObject::create_shader(Args&&... args)
{
  m_shader = std::make_unique<Shader>(std::forward<Args>(args)...);
}
