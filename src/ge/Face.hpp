#pragma once

#include <vector>
#include <glad/gl.h>

struct Face {

  Face() : size(0), data(nullptr) {}
  Face(const std::initializer_list<GLuint>& indices);
  Face(const std::vector<GLuint>& indices);
  Face(const Face& other);
  Face& operator=(const Face& other);
  Face(Face&& other) noexcept;
  Face& operator=(Face&& other) noexcept;
  void resize(int size);
  ~Face();
  
  int size;
  GLuint* data;  // can use vector instead but, every Face will be increased by sizeof(vector)
};


