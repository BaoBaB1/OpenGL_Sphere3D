#pragma once

#include "UserInputHandler.hpp"

class KeyboardHandler : public UserInputHandler {
public:
  enum InputKey {
    W = GLFW_KEY_W,
    A = GLFW_KEY_A,
    S = GLFW_KEY_S,
    D = GLFW_KEY_D,
    ARROW_UP = GLFW_KEY_UP,
    ARROW_DOWN = GLFW_KEY_DOWN,
    ARROW_LEFT = GLFW_KEY_LEFT,
    ARROW_RIGHT = GLFW_KEY_RIGHT
  };
  KeyboardHandler(MainWindow* window);
  int key_state(InputKey key) const;
private:
  void key_callback(int key, int scancode, int action, int mods);
  // key, state
  std::map<int, int> m_keys;
};
