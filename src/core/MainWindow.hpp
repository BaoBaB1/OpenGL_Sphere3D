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
  std::vector<UserInputHandler*>& input_handlers() { return m_input_handlers; }
  void remove(IObserver* observer);
  void add(IObserver* observer);
  void notify(IObserver* observer, bool enable);
  void notify_all(bool enable);
  int width() const { return m_width; }
  int height() const { return m_height; }
  const char* title() const { return m_title; }
private:
  const char* m_title;
  int m_width;
  int m_height;
  GLFWwindow* m_window;
  std::vector<UserInputHandler*> m_input_handlers;
  std::vector<IObserver*> m_observers;
};
