#include "Model.hpp"
#include "macro.hpp"

Model::Model(Type type) {
	m_model_mat = glm::mat4(1.f);
	m_color = glm::vec4(1.f);
	m_flags = 0;
	m_type = type;
	m_shading_mode = ShadingMode::NO_SHADING;
}

void Model::set_color(const glm::vec4& color) {
	m_color = color;
	for (Mesh& m : m_meshes) {
		for (Vertex& v : m.vertices()) {
			v.color = color;
		}
	}
}

void Model::apply_shading() {
	// maybe in future add general algorithms for other shading modes (depends on how other models will implement shading)
	if (m_shading_mode == NO_SHADING) {
		for (Mesh& m : m_meshes) {
			if (!m.is_auxiliary()) {
				std::vector<Vertex>& vertices = m.vertices();
				for (Vertex& v : vertices)
					v.normal = glm::vec3(0.f);
			}
		}
	}
}

void Model::normals(std::vector<GLfloat>& buffer) {
	buffer.clear();
	size_t size = 0;
	for (Mesh& m : m_meshes) {
		if (!m.is_auxiliary())
			size += m.vertices().size() * 6;
	}
	buffer.resize(size);
	size_t offset = 0; 
	const int len_scaler = 3;
	for (const Mesh& m : m_meshes) {
		if (!m.is_auxiliary()) {
			for (const Vertex& v : m.vertices()) {
				GLfloat line[6] = { v.position.x, v.position.y, v.position.z,
					v.position.x + v.normal.x / len_scaler, v.position.y + v.normal.y / len_scaler, v.position.z + v.normal.z / len_scaler };
				memcpy(buffer.data() + offset, line, sizeof(line));
				offset += 6;
			}
		}
	}	
}
