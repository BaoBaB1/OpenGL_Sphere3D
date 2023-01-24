#pragma once

#include <glad\glad.h>
#include <GLFW\glfw3.h>

class MainWindow
{
public:
	MainWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share);
	GLFWwindow* window() const { return m_window; }
	int width() const { return m_width; }
	int height() { return m_height; }
	~MainWindow();
private:
	GLFWwindow* m_window; 
	int m_width;
	int m_height;
};
