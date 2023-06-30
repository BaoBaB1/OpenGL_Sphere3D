#pragma once

#include <vector>
#include <memory>
#include "Shader.hpp"
#include "Camera.hpp"
#include "UserInputHandler.hpp"
#include "GPUBuffers.hpp"
#include "./ge/Object3D.hpp"

class MainWindow;

class SceneRenderer {
	using InputKey = KeyboardHandler::InputKey;
public:
	SceneRenderer();
	void render();
private:
	std::unique_ptr<MainWindow> m_window;
	std::unique_ptr<Shader> m_shader;
	std::unique_ptr<Camera> m_camera;
	std::unique_ptr<GPUBuffers> m_gpu_buffers;
	std::vector<std::unique_ptr<Object3D>> m_drawables;
	glm::mat4 m_projection_mat;
private:
	void handle_input();
	void render_scene();
	void create_scene();
};
