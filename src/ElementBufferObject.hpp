#pragma once

#include <glad/glad.h>

class ElementBufferObject {
public:
	ElementBufferObject();
	~ElementBufferObject();
	void set_data(const void* const indices, size_t size_in_bytes);
	void bind();
	void unbind();
	GLuint id() const { return m_id; }
private:
	GLuint m_id;
};
