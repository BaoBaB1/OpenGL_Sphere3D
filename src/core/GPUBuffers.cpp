#include "GPUBuffers.hpp"

GPUBuffers::GPUBuffers() {
	vao = new VertexArrayObject();
	vbo = new VertexBufferObject();
	ebo = new ElementBufferObject();
}

GPUBuffers::~GPUBuffers() {
	delete vao;
	delete vbo;
	delete ebo;
}

void GPUBuffers::bind_all() {
	vao->bind();
	vbo->bind();
	ebo->bind();
}

void GPUBuffers::unbind_all() {
	vao->unbind();
	vbo->unbind();
	ebo->unbind();
}
