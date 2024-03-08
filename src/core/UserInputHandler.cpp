#include <iostream>
#include <stdexcept>

#include "UserInputHandler.hpp"
#include "MainWindow.hpp"

std::map<UserInputHandler::HandlerType, void*> UserInputHandler::m_ptrs;

static std::string handler_type_to_string(UserInputHandler::HandlerType type)
{
  switch (type)
  {
  case UserInputHandler::KEYBOARD:
    return "Keyboard input";
  case UserInputHandler::CURSOR_POSITION:
    return "Cursor position";
  case UserInputHandler::MOUSE_INPUT:
    return "Mouse input";
  default:
    return "Unknown";
  }
}

UserInputHandler::UserInputHandler(MainWindow* window, HandlerType type)
{
  if (m_ptrs.find(type) != m_ptrs.end())
  {
    std::string msg = handler_type_to_string(type) + " handler already exists\n";
    throw std::runtime_error(msg);
  }
  m_type = type;
  m_window = window;
  m_ptrs[m_type] = this;
  m_disabled = false;
}

UserInputHandler::~UserInputHandler()
{
  m_ptrs.erase(m_type);
}

void UserInputHandler::notify(bool _enable)
{
  if (_enable)
    enable();
  else
    disable();
}
