#pragma once

#include <glad/glad.h>

class Shader
{
public:
	Shader(const char* const vertex_file, const char* const fragment_file);
	~Shader();
	void activate();
	GLuint id() { return m_id; }
private:
	GLuint m_id;
};
