#include "UserInputHandler.hpp"
#include "MainWindow.hpp"

std::map<InputType, void*> UserInputHandler::m_ptrs;

UserInputHandler::UserInputHandler(MainWindow* window, InputType input_type) {
  m_type = input_type;
  m_window = window;
  m_disabled = false;
}

void UserInputHandler::notify(bool _enable)
{
  if (_enable)
    enable();
  else
    disable();
}
