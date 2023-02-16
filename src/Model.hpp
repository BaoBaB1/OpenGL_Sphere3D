#pragma once

#include <glad/glad.h>
#include "Mesh.hpp"

class Model {
public:
	enum Type {
		//UNKNOWN = 0,
		ICOSAHEDRON = 1, // Icosahedron => Sphere
		CUBE
	};
	enum ShadingMode {
		NO_SHADING = 1,
		SMOOTH_SHADING,
		FLAT_SHADING,
	};
protected:
	enum Flag {
		ROTATABLE = (1 << 0),
		DRAW_NORMALS = (1 << 1),
		LIGHT_SOURCE = (1 << 2)
	};
public:
	virtual ~Model() {}
	virtual void apply_shading();
	void normals(std::vector<GLfloat>& buffer);

	Mesh& mesh(size_t i_mesh) { return m_meshes[i_mesh]; }
	const Mesh& mesh(size_t i_mesh) const { return m_meshes[i_mesh]; }
	std::vector<Mesh>& meshes() { return m_meshes; }
	const std::vector<Mesh>& meshes() const { return m_meshes; }
	glm::mat4& model_matrix() { return m_model_mat; }
	const glm::mat4& model_matrix() const { return m_model_mat; }
	glm::vec4& color() { return m_color; }
	const glm::vec4& color() const { return m_color; }
	Type type() const { return m_type; }
	ShadingMode shading_mode() const { return m_shading_mode; }

	void set_shading_mode(ShadingMode mode) { m_shading_mode = mode; }
	void set_color(const glm::vec4& color);
	void rotate(float angle, const glm::vec3& axis) { m_model_mat = glm::rotate(m_model_mat, glm::radians(angle), axis); }
	void scale(const glm::vec3& scale) { m_model_mat = glm::scale(m_model_mat, scale); }
	void translate(const glm::vec3& translation) { m_model_mat = glm::translate(m_model_mat, translation); }
	void light_source(bool val) { set_flag(LIGHT_SOURCE, val); }
	void rotatable(bool val) { set_flag(ROTATABLE, val); }
	void show_normals(bool val) { set_flag(DRAW_NORMALS, val); }
	bool is_show_normals() const { return get_flag(DRAW_NORMALS); }
	bool is_rotatable() const { return get_flag(ROTATABLE); }
	bool is_light_source() const { return get_flag(LIGHT_SOURCE); }
protected:
	Model(Type type);
	Model(const Model&) = default;
	Model(Model&&) = default;
	Model& operator=(const Model&) = default;
	Model& operator=(Model&&) = default;
	virtual void generate_initial_mesh() = 0;
	void set_flag(Flag flag, bool value) { value ? set_flag(flag) : clear_flag(flag); }
	void set_flag(Flag flag) { m_flags |= flag; }
	void clear_flag(Flag flag) { m_flags &= ~flag; }
	bool get_flag(Flag flag) const { return m_flags & flag; }
protected:
	std::vector<Mesh> m_meshes;
	glm::mat4 m_model_mat;
	glm::vec4 m_color;
	Type m_type;
	ShadingMode m_shading_mode;
	unsigned int m_flags;
};

