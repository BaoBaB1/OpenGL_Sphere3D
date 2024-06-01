#include "FrameBufferObject.hpp"

FrameBufferObject::FrameBufferObject(int w, int h) : m_width(w), m_height(h)
{
  glGenFramebuffers(1, id_ref());
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
  return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void FrameBufferObject::attach_renderbuffer(GLenum internalformat, GLenum attachment)
{
  if (m_render_buffer_id == 0)
  {
    glGenRenderbuffers(1, &m_render_buffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, m_render_buffer_id);
    glRenderbufferStorage(GL_RENDERBUFFER, internalformat, m_width, m_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, m_render_buffer_id);
  }
}

void FrameBufferObject::attach_texture(int w, int h, GLint internalformat, GLint format, GLint type)
{
  if (!m_texture)
  {
    m_texture = std::make_unique<Texture2D>(w, h, internalformat, format, type);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->id(), 0);
  }
}

FrameBufferObject::~FrameBufferObject()
{
  glDeleteFramebuffers(1, id_ref());
  glDeleteRenderbuffers(1, &m_render_buffer_id);
}
