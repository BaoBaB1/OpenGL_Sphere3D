#pragma once

#include "VertexArrayObject.hpp"
#include "VertexBufferObject.hpp"
#include "ElementBufferObject.hpp"

struct GPUBuffers {

  GPUBuffers();
  void bind_all();
  void unbind_all();
  ~GPUBuffers();

  VertexArrayObject* vao;
  VertexBufferObject* vbo;
  ElementBufferObject* ebo;
};
