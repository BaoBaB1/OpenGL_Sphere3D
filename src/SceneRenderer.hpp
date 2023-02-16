#pragma once

#include "VertexArrayObject.hpp"
#include "VertexBufferObject.hpp"
#include "ElementBufferObject.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "UserInputHandler.hpp"
#include "Model.hpp"

class MainWindow;

class SceneRenderer {
	using InputKey = KeyboardHandler::InputKey;
public:
	SceneRenderer(MainWindow* window);
	~SceneRenderer();
	void render();
private:
	MainWindow* m_window;
	Shader* m_shader;
	Camera* m_camera;
	VertexArrayObject* m_vao;
	VertexBufferObject* m_vbo;
	ElementBufferObject* m_ebo;
	std::vector<Model*> m_drawables;
	glm::mat4 m_projection_mat;
private:
	void handle_inputs();
	void bind_buffers();
	void unbind_buffers();
	void render_scene();
	void create_scene();
};

class FramesCounter {
public:
	FramesCounter(const char* window_title);
	double delta_time() { return m_delta_time; }
	void update_delta_time(double time);
	void update_fps(GLFWwindow* window, double time);
private:
	int m_frames;
	double m_last_fps_update;
	double m_last_frame;
	double m_delta_time;
	const char* m_window_title;
};