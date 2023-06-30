#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	Shader(const char* vertex_file, const char* fragment_file);
	~Shader();
	void set_matrix4f(const char* uniform_name, const glm::mat4& value);
	void set_vec3(const char* uniform_name, const glm::vec3& value);
	void set_bool(const char* uniform_name, bool value);
	void set_float(const char* uniform_name, float value);
	//void set_glsl_struct(const IStructGLSL* glsl_struct, const std::string& uniform_name);
	void activate();
	GLuint id() const { return m_id; }
private:
	GLuint m_id;
};
