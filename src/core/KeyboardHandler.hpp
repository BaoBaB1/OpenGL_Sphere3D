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
    ARROW_RIGHT = GLFW_KEY_RIGHT,
    LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT,
    UNKNOWN = 0xFFFFFFFF
  };

  enum KeyState
  {
    PRESSED,
    RELEASED,
    NO_STATE
  };

  static constexpr InputKey registered_keys[] = {W, A, S, D, ARROW_UP, ARROW_DOWN, ARROW_LEFT, ARROW_RIGHT, LEFT_SHIFT};

  KeyboardHandler(MainWindow* window);
  KeyboardHandler::KeyState get_keystate(KeyboardHandler::InputKey key);
private:
  void key_callback(int key, int scancode, int action, int mods);
  // key, state
  std::map<InputKey, KeyState> m_keystate;
};
