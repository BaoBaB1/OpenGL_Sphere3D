#include "MouseInputHandler.hpp"
#include "MainWindow.hpp"

MouseInputHandler::MouseInputHandler(MainWindow* window) : UserInputHandler(window, HandlerType::MOUSE_INPUT)
{
  auto callback = [](GLFWwindow* window, int button, int action, int mods)
    {
      if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
      {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        glfwSetWindowUserPointer(window, m_ptrs[HandlerType::MOUSE_INPUT]);
        auto p = static_cast<MouseInputHandler*>(glfwGetWindowUserPointer(window));
        p->m_x = static_cast<GLint>(x);
        p->m_y = p->m_window->height() - static_cast<GLint>(y) - 1;
        p->m_left_button_clicked = true;
      }
    };
  m_left_button_clicked = false;
  m_x = m_y = 0;
  glfwSetMouseButtonCallback(m_window->gl_window(), callback);
}
