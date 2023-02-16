#include "Vertex.hpp"

Vertex::Vertex() {
	position = glm::vec3(0.f);
	normal = glm::vec3(0.f);
	color = glm::vec4(0.f);
}

Vertex::Vertex(const glm::vec3& position) {
	this->position = position;
	normal = glm::vec3(0.f);
	color = glm::vec4(0.f);
}

Vertex::Vertex(const glm::vec3& position, const glm::vec3& normal) {
	this->position = position;
	this->normal = normal;
	color = glm::vec4(0.f);
}

Vertex::Vertex(const glm::vec3& position, const glm::vec4& color) {
	this->position = position;
	this->color = color;
	normal = glm::vec3(0.f);
}

Vertex::Vertex(const glm::vec3& position, const glm::vec4& color, const glm::vec3& normal) {
	this->position = position;
	this->normal = normal;
	this->color = color;
}

Vertex::Vertex(float x, float y, float z) {
	position.x = x; 
	position.y = y;
	position.z = z;
	normal = glm::vec3(0.f);
	color = glm::vec4(0.f);
}

Vertex Vertex::operator+(const Vertex& other) const {
	return Vertex((other.position + position), color);
}

Vertex Vertex::operator/(float value) const {
	return Vertex(position / 2.f, color);
}

bool Vertex::operator==(const Vertex& other) const {
	return color == other.color && position == other.position && normal == other.normal;
}
