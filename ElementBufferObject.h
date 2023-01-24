#pragma once

#include <glad/glad.h>

class ElementBufferObject {
public:
	ElementBufferObject();
	~ElementBufferObject();
	void set_data(GLuint* indices, size_t size_in_bytes);
	void bind();
	void unbind();
	GLuint id() { return m_id; }
private:
	GLuint m_id;
};
