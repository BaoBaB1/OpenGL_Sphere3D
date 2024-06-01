#include "Skybox.hpp"

void Skybox::render(GPUBuffers* gpu_buffers)
{
  auto vao = gpu_buffers->vao;
  auto vbo = gpu_buffers->vbo;
  vao->bind();
  vbo->bind();
  m_cubemap.bind();
  vbo->set_data(skyboxVertices, sizeof(skyboxVertices));
  vao->link_attrib(0, 3, GL_FLOAT, sizeof(float) * 3, nullptr);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  m_cubemap.unbind();
  vbo->unbind();
  vao->unbind();
}
