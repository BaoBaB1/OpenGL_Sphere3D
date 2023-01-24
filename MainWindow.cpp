#include "MainWindow.h"
#include "macro.h"

MainWindow::MainWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share) : 
	m_width(width), m_height(height) {
	// Initialize GLFW
	glfwInit();
	// Tell GLFW what version of OpenGL we are using 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	// Tell GLFW we are using the CORE profile (only modern functions)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	m_window = glfwCreateWindow(width, height, title, monitor, share);
	if (m_window == nullptr) {
		DEBUG("Failed to create GLFW window" << std::endl);
		glfwTerminate();
	}
}

MainWindow::~MainWindow() {
	glfwDestroyWindow(m_window);
	glfwTerminate();
}
