#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "VertexBufferObject.h"

class VertexArrayObject
{
public:
	VertexArrayObject();
	~VertexArrayObject();
	void link_attrib(GLuint layout, GLuint num_components, GLenum type, GLsizei stride, void* offset);
	void bind();
	void unbind();
	GLuint id() { return m_id; }
private:
	GLuint m_id;
};
