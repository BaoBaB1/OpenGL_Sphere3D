#include <stdexcept>

#include "MainWindow.hpp"
#include "KeyboardHandler.hpp"

KeyboardHandler::KeyboardHandler(MainWindow* window) : UserInputHandler(window, HandlerType::KEYBOARD)
{
  auto key_callback = [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
      glfwSetWindowUserPointer(window, m_ptrs[HandlerType::KEYBOARD]);
      static_cast<KeyboardHandler*>(glfwGetWindowUserPointer(window))->key_callback(key, scancode, action, mods);
    };
  for (InputKey key : KeyboardHandler::registered_keys)
  {
    m_keystate[key] = KeyState::NO_STATE;
  }
  glfwSetKeyCallback(m_window->gl_window(), key_callback);
}

void KeyboardHandler::key_callback(int key, int scancode, int action, int mods)
{
  for (auto reg_key : KeyboardHandler::registered_keys)
  {
    if (reg_key == key)
    {
      InputKey ckey = static_cast<InputKey>(key);
      switch (action)
      {
      case GLFW_RELEASE:
        m_keystate[ckey] = KeyState::RELEASED;
        break;
      case GLFW_PRESS:
        m_keystate[ckey] = KeyState::PRESSED;
        break;
      }
      break;
    }
  }
}

KeyboardHandler::KeyState KeyboardHandler::get_keystate(KeyboardHandler::InputKey key)
{
  KeyState last_state = m_keystate[key];
  if (last_state == KeyState::RELEASED)
  {
    // to prevent endless release state handling in input handler,
    // but can't use in sequential calls. bullshit design ? rework later
    m_keystate[key] = KeyState::NO_STATE;
  }
  return last_state;
}
