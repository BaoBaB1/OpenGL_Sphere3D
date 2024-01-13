#include "Polyline.hpp"

void Polyline::render(GPUBuffers* gpu_buffers, Shader* shader) 
{
  assert(gpu_buffers != nullptr && shader != nullptr);
  assert(m_mesh.vertices().size() > 0);
  shader->set_bool("applyShading", false);
  RenderConfig cfg;
  cfg.use_indices = false;
  cfg.apply_shading = false;
  cfg.mode = GL_LINE_STRIP;
  Object3D::render(gpu_buffers, shader, &cfg);
}

void Polyline::add(const Vertex& point) 
{
  m_mesh.append_vertex(point);
}
