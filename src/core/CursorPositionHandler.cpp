#include "CursorPositionHandler.hpp"
#include "SceneRenderer.hpp"
#include "MainWindow.hpp"

CursorPositionHandler::CursorPositionHandler(MainWindow* window) : UserInputHandler(window, HandlerType::CURSOR_POSITION)
{
  auto callback = [](GLFWwindow* window, double xpos, double ypos)
    {
      glfwSetWindowUserPointer(window, m_ptrs[HandlerType::CURSOR_POSITION]);
      static_cast<CursorPositionHandler*>(glfwGetWindowUserPointer(window))->callback(xpos, ypos);
    };
  glfwSetCursorPosCallback(m_window->gl_window(), callback);
  glfwGetCursorPos(m_window->gl_window(), &m_cur_pos[0], &m_cur_pos[1]);
  m_prev_pos[0] = m_prev_pos[1] = 0;
}

void CursorPositionHandler::callback(double xpos, double ypos)
{
  if (!m_disabled)
  {
    m_prev_pos[0] = m_cur_pos[0];
    m_prev_pos[1] = m_cur_pos[1];
    m_cur_pos[0] = xpos;
    m_cur_pos[1] = ypos;
    double diff_x = m_cur_pos[0] - m_prev_pos[0];
    double diff_y = m_cur_pos[1] - m_prev_pos[1];
    if (diff_x != 0.0 || diff_y != 0.0)
    {
      auto& scene = SceneRenderer::instance();
      scene.m_camera.add_to_yaw_and_pitch(diff_x, diff_y);
    }
  }
}
