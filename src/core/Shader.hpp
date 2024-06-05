#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "OpenGLObject.hpp"

class Shader : public OpenGLObject
{
public:
  OnlyMovable(Shader)
  Shader() = default;
  Shader(const char* vertex_file, const char* fragment_file);
  ~Shader();
  void load(const char* vertex_file, const char* fragment_file);
  void set_matrix4f(const char* uniform_name, const glm::mat4& value);
  void set_vec3(const char* uniform_name, const glm::vec3& value);
  void set_bool(const char* uniform_name, bool value);
  void set_uint(const char* uniform_name, unsigned int value);
  void set_float(const char* uniform_name, float value);
  void bind() const override;
  void unbind() const override;
};
