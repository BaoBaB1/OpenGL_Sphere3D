#include "Polyline.hpp"

void Polyline::render(GPUBuffers* gpu_buffers) 
{
  assert(m_mesh.vertices().size() > 0);
  RenderConfig cfg;
  cfg.use_indices = false;
  cfg.mode = GL_LINE_STRIP;
  Object3D::render(gpu_buffers, cfg);
}

void Polyline::add(const Vertex& point) 
{
  m_mesh.append_vertex(point);
}
