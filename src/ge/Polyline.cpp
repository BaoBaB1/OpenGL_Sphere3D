#include "Polyline.hpp"

Polyline::Polyline() {

}

void Polyline::render(GPUBuffers* gpu_buffers, Shader* shader) {
	assert(gpu_buffers != nullptr && shader != nullptr);
	assert(m_mesh.vertices().size() > 0);
	shader->set_bool("applyShading", false);
	gpu_buffers->bind_all();
	gpu_buffers->vbo->set_data(m_mesh.vertices().data(), sizeof(Vertex) * m_mesh.vertices().size());
	gpu_buffers->vao->link_attrib(0, 3, GL_FLOAT, sizeof(Vertex), nullptr);						  // position
	gpu_buffers->vao->link_attrib(2, 4, GL_FLOAT, sizeof(Vertex), (void*)(sizeof(GLfloat) * 6));  // color
	glDrawArrays(GL_LINE_STRIP, 0, m_mesh.vertices().size());
	gpu_buffers->unbind_all();
}

void Polyline::add(const Vertex& point) {
	m_mesh.append_vertex(point);
}
