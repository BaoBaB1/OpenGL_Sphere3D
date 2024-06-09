#pragma once

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <map>
#include <vector>
#include <memory>
#include "UserInputHandler.hpp"

class MainWindow
{
public:
  MainWindow(int width, int height, const char* title);
  ~MainWindow();
  GLFWwindow* gl_window() const { return m_window; }
  std::vector<std::unique_ptr<UserInputHandler>>& input_handlers() { return m_input_handlers; }
  void notify(IObserver* observer, bool enable);
  void notify_all(bool enable);
  void set_width(int w) { m_width = w; }
  void set_height(int h) { m_height = h; }
  int width() const { return m_width; }
  int height() const { return m_height; }
  const char* title() const { return m_title; }
private:
  const char* m_title;
  int m_width;
  int m_height;
  GLFWwindow* m_window;
  std::vector<std::unique_ptr<UserInputHandler>> m_input_handlers;
};
