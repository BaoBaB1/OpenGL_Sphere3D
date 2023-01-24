#include <assert.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SceneRenderer.h"
#include "MainWindow.h"
#include "Icosahedron.h"
#include "Shader.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "ElementBufferObject.h"
#include "macro.h"

class DepthChanger {
public:
	DepthChanger(int8_t min_depth, int8_t max_depth) {
		if (min_depth >= max_depth)
			throw std::runtime_error("Max depth <= min_depth");
		m_min_depth = min_depth;
		m_max_depth = max_depth;
		m_depth = min_depth;
		m_increase_depth = true;
	}
	int8_t next_depth() {
		if ((m_depth == m_max_depth && m_increase_depth) || (m_depth == m_min_depth && !m_increase_depth)) {
			m_increase_depth = !m_increase_depth;
		}
		m_increase_depth ? ++m_depth : --m_depth;
		return m_depth;
	}
	int8_t max_depth() { return m_max_depth; }
	int8_t min_depth() { return m_min_depth; }
private:
	int8_t m_depth;
	int8_t m_min_depth;
	int8_t m_max_depth;
	bool m_increase_depth;
};

SceneRenderer::SceneRenderer(MainWindow* main_window) {
	glfwMakeContextCurrent(main_window->window());
	// Load GLAD so it configures OpenGL
	gladLoadGL();
	glViewport(0, 0, main_window->width(), main_window->height());
	m_window = main_window->window();
	m_aspect_ratio = (float)main_window->height() / main_window->width();
	m_shader = new Shader("../default.vert", "../default.frag");
	m_vao = new VertexArrayObject();
	m_vbo = new VertexBufferObject();
	m_ebo = new ElementBufferObject();
}

SceneRenderer::~SceneRenderer() {
	// m_window will be destroyed in MainWindow destructor
	delete m_shader;
	delete m_vao;
	delete m_vbo;
	delete m_ebo;
}

void SceneRenderer::render() {
	float rotation = 0.f;
	double rotation_time = glfwGetTime();
	double subdivision_time = rotation_time;
	std::unique_ptr<Icosahedron> icosahedron = std::make_unique<Icosahedron>();
	std::unique_ptr<DepthChanger> depth_changer = std::make_unique<DepthChanger>(0, 3);
	m_shader->activate();
	fill_buffers(icosahedron.get()); // put initial data into buffer
	while (!glfwWindowShouldClose(m_window)) {
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT);
		// draw only outlines of objects
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		double current_time = glfwGetTime();
		if (current_time - subdivision_time >= 1.5) {
			subdivision_time = current_time;
			icosahedron->subdivide_triangles(depth_changer->next_depth());
			icosahedron->project_points_on_sphere();
			fill_buffers(icosahedron.get());
		}
		if (current_time - rotation_time >= 0.02) {
			rotation_time = current_time;
			rotation += 1.3f;
		}
		apply_matrices(rotation);
		bind_buffers();
		glDrawElements(GL_TRIANGLES, icosahedron->indices().size(), GL_UNSIGNED_INT, nullptr);
		unbind_buffers();
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(m_window);
		// Take care of all GLFW events
		glfwPollEvents();
	}
}

void SceneRenderer::apply_matrices(float model_rotation) {
	glm::mat4 model(1.f);
	glm::mat4 view(1.f);
	glm::mat4 projection(1.f);
	model = glm::rotate(model, glm::radians(model_rotation), glm::vec3(0.f, 1.f, 0.f));
	view = glm::translate(view, glm::vec3(0.f, 0.f, -5.f));
	// field of view angle, in degrees, in the y-direction
	projection = glm::perspective(glm::radians(45.f), m_aspect_ratio, 0.1f, 100.f);
	glUniformMatrix4fv(glGetUniformLocation(m_shader->id(), "MVP"), 1, GL_FALSE, glm::value_ptr(projection * view * model));
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

void SceneRenderer::fill_buffers(const Icosahedron* const ico) {
	std::vector<GLfloat> points_coordinates;
	std::vector<GLuint> indices = std::move(ico->indices());
	points_coordinates.resize(ico->mesh().size() * 3);
	size_t i = 0; 
	for (const Point3D& p : ico->mesh()) {
		points_coordinates[i++] = p.x;
		points_coordinates[i++] = p.y;
		points_coordinates[i++] = p.z;
	}
	//DEBUG("count points " << points_coordinates.size() / 3 << std::endl);
	//DEBUG("count faces " << indices.size() << std::endl);
	bind_buffers();
	m_vbo->set_data(&points_coordinates[0], sizeof(GLfloat) * points_coordinates.size());
	m_ebo->set_data(&indices[0], sizeof(GLuint) * indices.size());
	m_vao->link_attrib(0, 3, GL_FLOAT, sizeof(Point3D), nullptr);
	unbind_buffers();
}
