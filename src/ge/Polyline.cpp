#include "Polyline.hpp"

void Polyline::render(GPUBuffers* gpu_buffers) 
{
  assert(m_meshes[0].vertices().size() > 0);
  RenderConfig cfg;
  cfg.use_indices = false;
  cfg.mode = GL_LINE_STRIP;
  Object3D::render(gpu_buffers, cfg);
}

void Polyline::add(const Vertex& point) 
{
  m_meshes[0].append_vertex(point);
}
