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

void FrameBufferObject::attach_renderbuffer(GLenum internalformat, GLenum attachment)
{
  if (m_depth_buffer_id == 0)
  {
    glGenRenderbuffers(1, &m_depth_buffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depth_buffer_id);
    glRenderbufferStorage(GL_RENDERBUFFER, internalformat, m_width, m_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, m_depth_buffer_id);
  }
}

void FrameBufferObject::attach_texture(int w, int h, GLint internalformat, GLint format, GLint type)
{
  if (!m_texture)
  {
    m_texture = std::make_unique<Texture>(w, h, internalformat, format, type);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->id(), 0);
  }
}

FrameBufferObject::~FrameBufferObject()
{
  glDeleteFramebuffers(1, &m_id);
  glDeleteRenderbuffers(1, &m_depth_buffer_id);
}
