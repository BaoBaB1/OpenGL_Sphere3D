#pragma once

#include "UserInputHandler.hpp"

class MouseInputHandler : public UserInputHandler
{
public:
  OnlyMovable(MouseInputHandler)
  MouseInputHandler(MainWindow* window);
  int x() const { return m_x; }
  int y() const { return m_y; }
private:
  void left_btn_click_callback(GLFWwindow* window, int button, int action, int mods);
  void window_size_change_callback(GLFWwindow* window, int width, int height);
private:
  int m_x = 0;
  int m_y = 0;
};
