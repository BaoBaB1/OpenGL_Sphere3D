#include <assert.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>

#include "SceneRenderer.hpp"
#include "MainWindow.hpp"
#include "Icosahedron.hpp"
#include "Shader.hpp"
#include "VertexArrayObject.hpp"
#include "VertexBufferObject.hpp"
#include "ElementBufferObject.hpp"
#include "macro.hpp"
#include "Camera.hpp"
#include "UserInputHandler.hpp"
#include "Cube.hpp"

SceneRenderer::SceneRenderer(MainWindow* window) {
	m_window = window;
	m_shader = new Shader("default.vert", "default.frag");
	m_vao = new VertexArrayObject();
	m_vbo = new VertexBufferObject();
	m_ebo = new ElementBufferObject();
	m_camera = new Camera(glm::vec3(0.f, 0.f, 4.5f), 5.f, 0.2f);
	m_projection_mat = glm::mat4(1.f);
	m_projection_mat = glm::perspective(glm::radians(45.f), (float)m_window->height() / m_window->width(), 0.1f, 100.f);
	create_scene();
}

SceneRenderer::~SceneRenderer() {
	delete m_shader;
	delete m_vao;
	delete m_vbo;
	delete m_ebo;
	delete m_camera;
	for (Model* ent : m_drawables)
		delete ent;
}

void SceneRenderer::render() {
	double last_time = glfwGetTime();
	UpDownNumberChanger depth_changer(0, 2);
	FramesCounter frames_counter(m_window->title());
	GLFWwindow* gl_window = m_window->gl_window();
	m_shader->activate();
	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	while (!glfwWindowShouldClose(gl_window)) {
		handle_inputs();
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		double time = glfwGetTime();
		frames_counter.update_fps(gl_window, time);
		frames_counter.update_delta_time(time);
		m_camera->scale_speed(frames_counter.delta_time());
		render_scene();
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(gl_window);
		// Take care of all GLFW events
		glfwPollEvents();
	}
}

void SceneRenderer::render_scene() {
	for (const auto obj : m_drawables) {
		if (obj->is_rotatable()) {
			obj->rotate(1.f, glm::vec3(1.f, 1.f, 1.f));
		}
		if (obj->is_light_source() && obj->type() == Model::CUBE) {
			m_shader->set_vec3("lightPos", static_cast<Cube*>(obj)->center());
			m_shader->set_vec3("lightColor", static_cast<Cube*>(obj)->color());
			float x = std::sin(glfwGetTime());
			float z = std::cos(glfwGetTime());
			obj->translate(glm::vec3(-x / 22 , -z / 30, z / 22));
		}
		m_shader->set_vec3("viewPos", m_camera->position());
		m_shader->set_matrix4f("modelMatrix", obj->model_matrix());
		m_shader->set_matrix4f("viewMatrix", m_camera->view_matrix());
		m_shader->set_matrix4f("projectionMatrix", m_projection_mat);
		m_shader->set_bool("applyShading", obj->shading_mode() != Model::ShadingMode::NO_SHADING);
		const Mesh& mesh = obj->mesh(0);  // take all meshes which !is_auxiliary
		const std::vector<Vertex>& vertices = mesh.vertices();
		const std::vector<Face>& faces = mesh.faces();
		const Vertex* raw_v = vertices.data();
		std::vector<GLuint> indices;
		mesh.faces_as_indices(indices);
		bind_buffers();
		m_vbo->set_data(raw_v, sizeof(Vertex) * vertices.size());
		m_ebo->set_data(indices.data(), sizeof(GLuint) * indices.size());
		m_vao->link_attrib(0, 3, GL_FLOAT, sizeof(Vertex), nullptr); // position
		m_vao->link_attrib(1, 3, GL_FLOAT, sizeof(Vertex), (void*)(sizeof(GLfloat) * 3)); // normal
		m_vao->link_attrib(2, 4, GL_FLOAT, sizeof(Vertex), (void*)(sizeof(GLfloat) * 6)); // color
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
		if (obj->is_show_normals()) {
			std::vector<GLfloat> normals;
			obj->normals(normals);
			m_vbo->set_data(normals.data(), sizeof(GLfloat) * normals.size());
			m_vao->link_attrib(0, 3, GL_FLOAT, sizeof(GLfloat) * 3, nullptr); // position
			glDrawArrays(GL_LINES, 0, normals.size());
		}
		unbind_buffers();
	}
}

void SceneRenderer::create_scene() {
	for (int i = 0; i < 2; ++i) {
		Icosahedron* ico = new Icosahedron();
		if (i == 0)
			ico->translate(glm::vec3(0.8f, 0.f, 0.f));
		else 
			ico->translate(glm::vec3(-0.8f, 0.f, 0.f));
		ico->set_color(glm::vec4(0.2f, 0.3f, 0.2f, 1.f));
		ico->scale(glm::vec3(0.5f, 0.5f, 0.5f));
		ico->set_shading_mode(i == 0 ? Model::ShadingMode::SMOOTH_SHADING : Model::ShadingMode::FLAT_SHADING);
		ico->subdivide_triangles(4);
		ico->project_points_on_sphere();
		ico->apply_shading();
		m_drawables.push_back(ico);
	}

	for (int i = 0; i < 2; ++i) {
		Cube* cube = new Cube();
		//cube->show_normals(true);
		if (i == 0) {
			cube->light_source(true);
			cube->set_color(glm::vec4(1.f, 1.f, 1.f, 1.f));
			cube->translate(glm::vec3(2.f, 0.f, -0.5f));
		}
		else {
			cube->set_color(glm::vec4(0.5f, 0.2f, 0.9f, 1.f));
			cube->set_shading_mode(Model::ShadingMode::FLAT_SHADING);
			cube->translate(glm::vec3(-0.15f, 0.f, 0.5f));
			cube->rotatable(true);
		}
		cube->scale(glm::vec3(0.3f, 0.3f, 0.3f));
		m_drawables.push_back(cube);
	}
}

void SceneRenderer::handle_inputs() {
	for (UserInputHandler* handler : m_window->input_handlers()) {
		switch (handler->type())
		{
		case InputType::KEYBOARD:
		{
			KeyboardHandler* kh = static_cast<KeyboardHandler*>(handler);
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
			CursorHandler* ch = static_cast<CursorHandler*>(handler);
			double x, y;
			ch->xy_offset(x, y);
			m_camera->add_to_yaw_and_pitch(x, y);
		}
		break;
		default:
			break;
		}
	}
}

void SceneRenderer::bind_buffers() {
	m_vao->bind();
	m_vbo->bind();
	m_ebo->bind();
}

void SceneRenderer::unbind_buffers() {
	m_vao->unbind();
	m_vbo->unbind();
	m_ebo->unbind();
}

FramesCounter::FramesCounter(const char* window_title) {
	m_window_title = window_title;
	m_last_fps_update = glfwGetTime();
	m_last_frame = m_last_fps_update;
	m_delta_time = 0.01;
	m_frames = 0;
}

void FramesCounter::update_delta_time(double time) {
	// time between 2 frames
	m_delta_time = time - m_last_frame;
	m_last_frame = time;
}

void FramesCounter::update_fps(GLFWwindow* window, double time) {
	if (time - m_last_fps_update > 1.) {
		std::stringstream ss;
		double fps = (m_frames) / (time - m_last_fps_update);
		ss << m_window_title << " [FPS = " << fps << " ]";
		glfwSetWindowTitle(window, ss.str().c_str());
		m_frames = 0;
		m_last_fps_update = time;
	}
	else {
		++m_frames;
	}
}
