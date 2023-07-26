#include <stdexcept>
#include "CursorHandler.hpp"
#include "MainWindow.hpp"

CursorHandler::CursorHandler(MainWindow* window) : UserInputHandler(window, InputType::CURSOR) 
{
  if (m_ptrs.find(this->type()) != m_ptrs.end()) 
    throw std::runtime_error("Cursor input handler already exists");
  m_ptrs[this->type()] = this;
  auto callback = [](GLFWwindow* window, double xpos, double ypos) 
  {
    glfwSetWindowUserPointer(window, m_ptrs[InputType::CURSOR]);
    static_cast<CursorHandler*>(glfwGetWindowUserPointer(window))->cursor_callback(xpos, ypos);
  };
  glfwSetCursorPosCallback(m_window->gl_window(), callback);
  glfwGetCursorPos(m_window->gl_window(), &m_cur_pos[0], &m_cur_pos[1]);
  m_prev_pos[0] = m_prev_pos[1] = 0;
  m_changed = m_renew_callback = false;
}

CursorHandler::~CursorHandler() 
{
  m_ptrs.erase(InputType::CURSOR);
}

void CursorHandler::enable() 
{
  UserInputHandler::enable();
}

void CursorHandler::disable() 
{
  UserInputHandler::disable();
  m_renew_callback = true;
}

void CursorHandler::cursor_callback(double xpos, double ypos) 
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

void CursorHandler::xy_offset(double& x, double& y) 
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