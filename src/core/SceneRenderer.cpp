#include <assert.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>

#include "SceneRenderer.hpp"
#include "MainWindow.hpp"
#include "Shader.hpp"
#include "VertexArrayObject.hpp"
#include "VertexBufferObject.hpp"
#include "ElementBufferObject.hpp"
#include "macro.hpp"
#include "Camera.hpp"
#include "UserInputHandler.hpp"
#include "FramesCounter.hpp"
#include "./ge/Cube.hpp"
#include "./ge/Icosahedron.hpp"
#include "./ge/Polyline.hpp"
#include "./ge/Pyramid.hpp"

SceneRenderer::SceneRenderer() {
	m_window = std::make_unique<MainWindow>(800, 800, "OpenGLWindow");
	m_shader = std::make_unique<Shader>("./src/glsl/vertex_shader", "./src/glsl/fragment_shader");
	m_gpu_buffers = std::make_unique<GPUBuffers>();
	m_camera = std::make_unique<Camera>();
	m_camera->set_position(glm::vec3(-4.f, 2.f, 3.f));
	m_camera->look_at(glm::vec3(2.f, 0.5f, 0.5f));
	m_projection_mat = glm::mat4(1.f);
	m_projection_mat = glm::perspective(glm::radians(45.f), (float)m_window->height() / m_window->width(), 0.1f, 100.f);
}

void SceneRenderer::render() {
	double last_time = glfwGetTime();
	FramesCounter fps_counter(m_window->title());
	GLFWwindow* gl_window = m_window->gl_window();
	m_shader->activate();
	create_scene();
	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	while (!glfwWindowShouldClose(gl_window)) {
		handle_input();
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		double time = glfwGetTime();
		fps_counter.update_fps(gl_window, time);
		fps_counter.update_delta_time(time);
		m_camera->scale_speed(fps_counter.delta_time());
		render_scene();
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(gl_window);
		// Take care of all GLFW events
		glfwPollEvents();
	}
}

void SceneRenderer::render_scene() {
	for (const auto& obj : m_drawables) {
		m_shader->set_vec3("viewPos", m_camera->position());
		m_shader->set_matrix4f("modelMatrix", obj->model_matrix());
		m_shader->set_matrix4f("viewMatrix", m_camera->view_matrix());
		m_shader->set_matrix4f("projectionMatrix", m_projection_mat);
		obj->render(m_gpu_buffers.get(), m_shader.get());
	}
}

void SceneRenderer::create_scene() {
	Vertex arr[6];
	arr[0].position = glm::vec3(0.f, 1.f, 0.f), arr[0].color = glm::vec4(0.f, 1.f, 0.f, 1.f);
	arr[1].position = glm::vec3(0.f, 0.f, 0.f), arr[1].color = glm::vec4(0.f, 1.f, 0.f, 1.f);
	arr[2].position = glm::vec3(0.f, 0.f, 0.f), arr[2].color = glm::vec4(1.f, 0.f, 0.f, 1.f);
	arr[3].position = glm::vec3(1.f, 0.f, 0.f), arr[3].color = glm::vec4(1.f, 0.f, 0.f, 1.f);
	arr[4].position = glm::vec3(0.f, 0.f, 0.f), arr[4].color = glm::vec4(0.f, 0.f, 1.f, 1.f);
	arr[5].position = glm::vec3(0.f, 0.f, 1.f), arr[5].color = glm::vec4(0.f, 0.f, 1.f, 1.f);
	std::unique_ptr<Polyline> origin = std::make_unique<Polyline>();
	for (int i = 0; i < 6; i++) {
		origin->add(arr[i]);
	}
	m_drawables.push_back(std::move(origin));

	std::unique_ptr<Icosahedron> sun = std::make_unique<Icosahedron>();
	sun->light_source(true);
	sun->translate(glm::vec3(4.f, -0.5f, 2.f));
	sun->set_color(glm::vec4(1.f, 1.f, 0.f, 1.f));
	sun->scale(glm::vec3(0.3f));
	sun->subdivide_triangles(4);
	sun->project_points_on_sphere();
	m_drawables.push_back(std::move(sun));

	std::unique_ptr<Icosahedron> sphere = std::make_unique<Icosahedron>();
	sphere->translate(glm::vec3(2.5f, 0.5f, 2.f));
	sphere->set_color(glm::vec4(1.f, 0.f, 0.f, 1.f));
	sphere->subdivide_triangles(4);
	sphere->project_points_on_sphere();
	sphere->scale(glm::vec3(0.3f));
	sphere->apply_shading(IShaderable::ShadingMode::SMOOTH_SHADING);
	m_drawables.push_back(std::move(sphere));

	std::unique_ptr<Cube> c = std::make_unique<Cube>();
	c->translate(glm::vec3(0.5f, 0.f, 0.5f));
	c->scale(glm::vec3(0.5f));
	c->apply_shading(IShaderable::ShadingMode::FLAT_SHADING);
	c->set_texture(".\\.\\src\\textures\\brick.jpg");
	m_drawables.push_back(std::move(c));

	std::unique_ptr<Cube> c2 = std::make_unique<Cube>();
	c2->translate(glm::vec3(2.f, 0.f, 0.5f));
	c2->set_color(glm::vec4(0.4f, 1.f, 0.4f, 1.f));
	c2->apply_shading(IShaderable::ShadingMode::FLAT_SHADING);
	c2->visible_normals(true);
	m_drawables.push_back(std::move(c2));

	std::unique_ptr<Pyramid> pyr = std::make_unique<Pyramid>();
	pyr->translate(glm::vec3(0.5f, 0.f,	2.f));
	pyr->scale(glm::vec3(0.5f));
	pyr->set_color(glm::vec4(0.976f, 0.212f, 0.98f, 1.f));
	pyr->apply_shading(IShaderable::ShadingMode::FLAT_SHADING);
	m_drawables.push_back(std::move(pyr));
}

void SceneRenderer::handle_input() {
	for (const auto& handler: m_window->input_handlers()) {
		switch (handler->type())
		{
		case InputType::KEYBOARD:
		{
			KeyboardHandler* kh = static_cast<KeyboardHandler*>(handler.get());
			if (kh->key_state(InputKey::W) == GLFW_PRESS || kh->key_state(InputKey::ARROW_UP) == GLFW_PRESS)
				m_camera->move(Camera::Direction::FORWARD);
			if (kh->key_state(InputKey::A) == GLFW_PRESS || kh->key_state(InputKey::ARROW_LEFT) == GLFW_PRESS)
				m_camera->move(Camera::Direction::LEFT);
			if (kh->key_state(InputKey::S) == GLFW_PRESS || kh->key_state(InputKey::ARROW_DOWN) == GLFW_PRESS)
				m_camera->move(Camera::Direction::BACKWARD);
			if (kh->key_state(InputKey::D) == GLFW_PRESS || kh->key_state(InputKey::ARROW_RIGHT) == GLFW_PRESS)
				m_camera->move(Camera::Direction::RIGHT);
		}
		break;
		case InputType::CURSOR:
		{
			CursorHandler* ch = static_cast<CursorHandler*>(handler.get());
			double x, y;
			ch->xy_offset(x, y);
			if (x != 0. || y != 0.)
				m_camera->add_to_yaw_and_pitch(x, y);
		}
		break;
		default:
			break;
		}
	}
}
