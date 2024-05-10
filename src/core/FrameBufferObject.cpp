#include "FrameBufferObject.hpp"

FrameBufferObject::FrameBufferObject(int w, int h) : m_width(w), m_height(h)
{
  glGenFramebuffers(1, &m_id);
}

void FrameBufferObject::bind() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void FrameBufferObject::unbind() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool FrameBufferObject::is_complete() const
{
  bind();
  bool complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
  unbind();
  return complete;
}

void FrameBufferObject::create_depth_buffer()
{
  // depth buffer
  if (m_depth_buffer_id)
    glDeleteRenderbuffers(1, &m_depth_buffer_id);
  glGenRenderbuffers(1, &m_depth_buffer_id);
}

void FrameBufferObject::create_texture(int w, int h, GLint internalformat, GLint format, GLint type)
{
  m_texture = std::make_unique<Texture>(w, h, internalformat, format, type);
}

void FrameBufferObject::attach_depth_buffer()
{
  if (m_depth_buffer_id)
  {
    glBindRenderbuffer(GL_RENDERBUFFER, m_depth_buffer_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth_buffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
  }
}

void FrameBufferObject::attach_current_texture()
{
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->id(), 0);
}

FrameBufferObject::~FrameBufferObject()
{
  glDeleteFramebuffers(1, &m_id);
  glDeleteRenderbuffers(1, &m_depth_buffer_id);
}
