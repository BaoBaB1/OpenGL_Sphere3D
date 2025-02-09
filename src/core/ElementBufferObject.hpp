#pragma once

#include <glad/gl.h>
#include "OpenGLObject.hpp"

class ElementBufferObject : public OpenGLObject 
{
public:
  OnlyMovable(ElementBufferObject)
  ElementBufferObject();
  ~ElementBufferObject();
  void set_data(void* indices, size_t size_in_bytes);
  void bind() const override;
  void unbind() const override;
};
