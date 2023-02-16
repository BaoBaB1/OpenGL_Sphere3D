#pragma once

#include "Model.hpp"

class Cube : public Model {
public:
	Cube();
	Cube(const Cube&) = default;
	Cube& operator=(const Cube&) = default;
	Cube(Cube&& other) noexcept;
	Cube& operator=(Cube&& other) noexcept;
	~Cube() {}

	glm::vec3 center() const;
	void apply_shading() override;
private:
	std::vector<glm::vec3> m_cached_normals;
private:
	void generate_initial_mesh() override;
};
