#include <string>
#include <fstream>
#include <exception>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"

static bool read_shader_file_content(const char* const file, std::string& content) {
	content.clear();
	std::ifstream in;
	in.open(file, std::ios_base::binary);
	if (in.is_open()) {
		in.seekg(0, std::ios::end);
		size_t sz = in.tellg();
		content.resize(sz);
		in.seekg(0, std::ios::beg);
		in.read(&content[0], sz);
		return true;
	}
	return false;
}

Shader::Shader(const char* const vertex_file, const char* const fragment_file) {
	std::string vertex_shader_source, fragment_shader_source; 
	if (!read_shader_file_content(vertex_file, vertex_shader_source))
		throw std::runtime_error("Error reading vertex shader file");
	if (!read_shader_file_content(fragment_file, fragment_shader_source))
		throw std::runtime_error("Error reading fragment shader file");

	const char* vss = vertex_shader_source.c_str();
	const char* fss = fragment_shader_source.c_str();

	// Create Vertex Shader Object and get its reference
	GLuint m_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	// Attach Vertex Shader source to the Vertex Shader Object
	glShaderSource(m_vertex_shader, 1, &vss, NULL);
	glCompileShader(m_vertex_shader);

	// Create Fragment Shader Object and get its reference
	GLuint m_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	// Attach Fragment Shader source to the Fragment Shader Object
	glShaderSource(m_fragment_shader, 1, &fss, NULL);
	glCompileShader(m_fragment_shader);

	// Create Shader Program Object and get its reference
	m_id = glCreateProgram();
	glAttachShader(m_id, m_vertex_shader);
	glAttachShader(m_id, m_fragment_shader);
	// Wrap-up/Link all the shaders together into the Shader Program
	glLinkProgram(m_id);

	// Delete the now useless Vertex and Fragment Shader objects
	glDeleteShader(m_vertex_shader);
	glDeleteShader(m_fragment_shader);
}

void Shader::activate() {
	glUseProgram(m_id);
}

Shader::~Shader() {
	glDeleteProgram(m_id);
}
