#pragma once

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <map>
#include <vector>
#include "UserInputHandler.hpp"

class SceneRenderer;

class MainWindow
{
public:
	MainWindow(int width, int height, const char* title);
	~MainWindow();
	GLFWwindow* gl_window() const { return m_window; }
	std::vector<UserInputHandler*> input_handlers() { return m_input_handlers; }
	int width() const { return m_width; }
	int height() const { return m_height; }
	const char* title() const { return m_title; }
private:
	GLFWwindow* m_window; 
	const char* m_title;
	int m_width;
	int m_height;
	std::vector<UserInputHandler*> m_input_handlers;
};
