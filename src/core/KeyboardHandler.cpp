#include <stdexcept>
#include "MainWindow.hpp"
#include "KeyboardHandler.hpp"

KeyboardHandler::KeyboardHandler(MainWindow* window) : UserInputHandler(window, InputType::KEYBOARD) 
{
  if (m_ptrs.find(this->type()) != m_ptrs.end())
    throw std::runtime_error("Keyboard input handler already exists");
  m_ptrs[this->type()] = this;
  auto key_callback = [](GLFWwindow* window, int key, int scancode, int action, int mods) 
  {
    glfwSetWindowUserPointer(window, m_ptrs[InputType::KEYBOARD]);
    static_cast<KeyboardHandler*>(glfwGetWindowUserPointer(window))->key_callback(key, scancode, action, mods);
  };
  glfwSetKeyCallback(m_window->gl_window(), key_callback);
}

KeyboardHandler::~KeyboardHandler() 
{
  m_ptrs.erase(InputType::KEYBOARD);
}

void KeyboardHandler::key_callback(int key, int scancode, int action, int mods) 
{
  if (key == W || key == A || key == S || key == D ||
    key == ARROW_LEFT || key == ARROW_RIGHT || key == ARROW_UP || key == ARROW_DOWN) {
    int state = glfwGetKey(m_window->gl_window(), key);
    m_keys[key] = state;
  }
}

int KeyboardHandler::key_state(InputKey key) const 
{
  auto iter = m_keys.find(key);
  if (iter != m_keys.end())
    return iter->second;
  return -1;
}
