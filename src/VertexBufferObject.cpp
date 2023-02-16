#include "VertexBufferObject.hpp"

VertexBufferObject::VertexBufferObject() {
	glGenBuffers(1, &m_id);
}

void VertexBufferObject::set_data(const void* const vertices, size_t size_in_bytes) {
	glBufferData(GL_ARRAY_BUFFER, size_in_bytes, vertices, GL_STATIC_DRAW);
}

void VertexBufferObject::bind() {
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VertexBufferObject::unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBufferObject::~VertexBufferObject() {
	glDeleteBuffers(1, &m_id);
}
