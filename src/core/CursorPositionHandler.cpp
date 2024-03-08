#include "CursorPositionHandler.hpp"
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
  m_changed = m_renew_callback = false;
}

void CursorPositionHandler::disable()
{
  UserInputHandler::disable();
  m_renew_callback = true;
}

void CursorPositionHandler::callback(double xpos, double ypos)
{
  if (!m_disabled)
  {
    if (m_renew_callback)
    {
      // m_cur_pos will store x,y values BEFORE handler has been disabled, 
      // so to prevent camera 'jumps' after it has been enabled again, set cursor position to old values
      glfwSetCursorPos(m_window->gl_window(), m_cur_pos[0], m_cur_pos[1]);
      m_renew_callback = false;
    }
    else
    {
      m_prev_pos[0] = m_cur_pos[0];
      m_prev_pos[1] = m_cur_pos[1];
      m_cur_pos[0] = xpos;
      m_cur_pos[1] = ypos;
      m_changed = true;
    }
  }
}

void CursorPositionHandler::xy_offset(double& x, double& y)
{
  if (m_changed)
  {
    x = m_cur_pos[0] - m_prev_pos[0];
    y = m_cur_pos[1] - m_prev_pos[1];
    m_changed = false;  // to avoid moving every frame
  }
  else
  {
    x = y = 0;
  }
}
