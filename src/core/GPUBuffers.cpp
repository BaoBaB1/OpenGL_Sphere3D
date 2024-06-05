#include "GPUBuffers.hpp"

GPUBuffers::GPUBuffers() 
{
  vao = std::make_unique<VertexArrayObject>();
  vbo = std::make_unique<VertexBufferObject>();
  ebo = std::make_unique<ElementBufferObject>();
}

void GPUBuffers::bind_all() 
{
  vao->bind();
  vbo->bind();
  ebo->bind();
}

void GPUBuffers::unbind_all() 
{
  vao->unbind();
  vbo->unbind();
  ebo->unbind();
}
