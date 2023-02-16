#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Vertex {
	Vertex();
	Vertex(const glm::vec3& position);
	Vertex(const glm::vec3& position, const glm::vec3& normal);
	Vertex(const glm::vec3& position, const glm::vec4& color);
	Vertex(const glm::vec3& position, const glm::vec4& color, const glm::vec3& normal);
	Vertex(float x, float y, float z);
	Vertex operator+(const Vertex& other) const;
	Vertex operator/(float value) const;
	bool operator==(const Vertex& other) const;
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec4 color;
};


