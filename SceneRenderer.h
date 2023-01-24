#pragma once

#include "MainWindow.h"
#include "Icosahedron.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "ElementBufferObject.h"
#include "Shader.h"

class SceneRenderer {
public:
	SceneRenderer(MainWindow* window);
	~SceneRenderer();
	void render();
private:
	float m_aspect_ratio;
	GLFWwindow* m_window;
	Shader* m_shader;
	VertexArrayObject* m_vao;
	VertexBufferObject* m_vbo;
	ElementBufferObject* m_ebo;
private:
	void bind_buffers();
	void unbind_buffers();
	void fill_buffers(const Icosahedron* const ico);
	void apply_matrices(float model_rotation);
};