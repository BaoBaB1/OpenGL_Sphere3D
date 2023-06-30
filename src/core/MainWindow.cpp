#include "MainWindow.hpp"
#include "macro.hpp"

MainWindow::MainWindow(int width, int height, const char* title) :
	m_width(width), m_height(height), m_title(title) {
	glfwInit();
	// Tell GLFW what version of OpenGL we are using 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	// Tell GLFW we are using the CORE profile (only modern functions)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (m_window == nullptr) {
		DEBUG("Failed to create GLFW window" << std::endl);
		glfwTerminate();
	}
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(m_window, m_width / 2., m_height / 2.);
	m_input_handlers.emplace_back(std::unique_ptr<KeyboardHandler>(new KeyboardHandler(this)));
	m_input_handlers.emplace_back(std::unique_ptr<CursorHandler>(new CursorHandler(this)));
	glfwMakeContextCurrent(m_window);
	gladLoadGL();
	glViewport(0, 0, m_width, m_height);
}

MainWindow::~MainWindow() {
	glfwDestroyWindow(m_window);
	glfwTerminate();
}
