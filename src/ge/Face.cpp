#include "Face.hpp"

Face::Face(const Face& other) {
  size = other.size;
  data = new GLuint[size];
  memcpy(data, other.data, sizeof(GLuint) * size);
}

Face& Face::operator=(const Face& other) {
  if (this != &other) {
    delete[] data;
    size = other.size;
    data = new GLuint[size];
    memcpy(data, other.data, sizeof(GLuint) * size);
  }
  return *this;
}

Face::Face(Face&& other) noexcept {
  size = other.size;
  data = other.data;
  other.data = nullptr;
  other.size = 0;
}

Face& Face::operator=(Face&& other) noexcept {
  if (this != &other) {
    delete[] data;
    size = other.size;
    data = other.data;
    other.data = nullptr;
    other.size = 0; 
  }
  return *this;
}

Face::~Face() {
  delete[] data;
}

Face::Face(const std::initializer_list<GLuint>& _indices) {
  size = _indices.size();
  data = new GLuint[size];
  memcpy(data, _indices.begin(), sizeof(GLuint) * size);
}

Face::Face(const std::vector<GLuint>& _indices) {
  size = _indices.size();
  data = new GLuint[size];
  memcpy(data, _indices.data(), sizeof(GLuint) * size);
}
