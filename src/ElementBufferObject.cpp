#include "ElementBufferObject.hpp"

ElementBufferObject::ElementBufferObject() {
	glGenBuffers(1, &m_id);
}

void ElementBufferObject::set_data(const void* const indices, size_t size_in_bytes) {
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_in_bytes, indices, GL_STATIC_DRAW);
}

void ElementBufferObject::bind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

void ElementBufferObject::unbind() {
	// MAKE SURE TO UNBIND IT AFTER UNBINDING THE VAO, as the EBO is linked in the VAO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

ElementBufferObject::~ElementBufferObject() {
	glDeleteBuffers(1, &m_id);
}
