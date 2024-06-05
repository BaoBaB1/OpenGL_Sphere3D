#pragma once

#include <memory>
#include "VertexArrayObject.hpp"
#include "VertexBufferObject.hpp"
#include "ElementBufferObject.hpp"

struct GPUBuffers 
{
  GPUBuffers();
  void bind_all();
  void unbind_all();

  std::unique_ptr<VertexArrayObject> vao;
  std::unique_ptr<VertexBufferObject> vbo;
  std::unique_ptr<ElementBufferObject> ebo;
};
