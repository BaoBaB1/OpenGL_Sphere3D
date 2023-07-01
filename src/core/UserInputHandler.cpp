#include "UserInputHandler.hpp"
#include "MainWindow.hpp"
#include "macro.hpp"

std::map<InputType, void*> UserInputHandler::m_ptrs;

UserInputHandler::UserInputHandler(MainWindow* window, InputType input_type) {
  m_type = input_type;
  m_window = window;
}

KeyboardHandler::KeyboardHandler(MainWindow* window) : UserInputHandler(window, InputType::KEYBOARD) {
  if (m_ptrs.find(this->type()) != m_ptrs.end()) {
    throw std::runtime_error("Keyboard input handler already exists");
  }
  m_ptrs[this->type()] = this;
  auto key_callback = [](GLFWwindow* window, int key, int scancode, int action, int mods) {
    glfwSetWindowUserPointer(window, m_ptrs[InputType::KEYBOARD]);
    static_cast<KeyboardHandler*>(glfwGetWindowUserPointer(window))->key_callback(key, scancode, action, mods);
  };
  glfwSetKeyCallback(m_window->gl_window(), key_callback);
}

KeyboardHandler::~KeyboardHandler() {
  m_ptrs.erase(InputType::KEYBOARD);
}

void KeyboardHandler::key_callback(int key, int scancode, int action, int mods) {
  if (key == W || key == A || key == S || key == D ||
    key == ARROW_LEFT || key == ARROW_RIGHT || key == ARROW_UP || key == ARROW_DOWN) {
    int state = glfwGetKey(m_window->gl_window(), key);
    m_keys[key] = state;
  }
}

int KeyboardHandler::key_state(InputKey key) const {
  auto iter = m_keys.find(key);
  if (iter != m_keys.end())
    return iter->second;
  return -1;
}

CursorHandler::CursorHandler(MainWindow* window) : UserInputHandler(window, InputType::CURSOR) {
  if (m_ptrs.find(this->type()) != m_ptrs.end()) {
    throw std::runtime_error("Cursor input handler already exists");
  }
  m_ptrs[this->type()] = this;
  auto cursor_callback = [](GLFWwindow* window, double xpos, double ypos) {
    glfwSetWindowUserPointer(window, m_ptrs[InputType::CURSOR]);
    static_cast<CursorHandler*>(glfwGetWindowUserPointer(window))->cursor_callback(xpos, ypos);
  };
  glfwSetCursorPosCallback(m_window->gl_window(), cursor_callback);
  glfwGetCursorPos(m_window->gl_window(), &m_cur_pos[0], &m_cur_pos[1]);
  m_changed = false;
}

CursorHandler::~CursorHandler() {
  m_ptrs.erase(InputType::CURSOR);
}

void CursorHandler::cursor_callback(double xpos, double ypos) {
  m_prev_pos[0] = m_cur_pos[0];
  m_prev_pos[1] = m_cur_pos[1];
  m_cur_pos[0] = xpos;
  m_cur_pos[1] = ypos;
  m_changed = true;
}

void CursorHandler::xy_offset(double& x, double& y) {
  x = y = 0.;
  if (m_changed) {
    x = m_cur_pos[0] - m_prev_pos[0];
    y = m_cur_pos[1] - m_prev_pos[1];
    m_changed = false;  // to avoid moving every frame
  }
}