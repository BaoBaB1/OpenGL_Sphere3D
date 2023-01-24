#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class VertexBufferObject {
public:
	VertexBufferObject();
	~VertexBufferObject();
	void set_data(GLfloat* vertices, size_t size_in_bytes);
	void bind();
	void unbind();
	GLuint id() { return m_id; }
private:
	GLuint m_id;
};