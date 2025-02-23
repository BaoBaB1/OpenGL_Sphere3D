#pragma once

#include "UserInputHandler.hpp"

class KeyboardHandler : public UserInputHandler 
{
public:
  enum InputKey : uint16_t
  {
    W = GLFW_KEY_W,
    A = GLFW_KEY_A,
    S = GLFW_KEY_S,
    D = GLFW_KEY_D,
    T = GLFW_KEY_T, 
    R = GLFW_KEY_R,
    ARROW_UP = GLFW_KEY_UP,
    ARROW_DOWN = GLFW_KEY_DOWN,
    ARROW_LEFT = GLFW_KEY_LEFT,
    ARROW_RIGHT = GLFW_KEY_RIGHT,
    LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT,
    SPACE = GLFW_KEY_SPACE,
    LEFT_CTRL = GLFW_KEY_LEFT_CONTROL,
    ESC = GLFW_KEY_ESCAPE,
    UNKNOWN = 0xFFFF
  };

  enum KeyState
  {
    PRESSED,
    RELEASED,
    NO_STATE
  };

  static constexpr InputKey registered_keys[] = {
    W, A, S, D, ARROW_UP, ARROW_DOWN, ARROW_LEFT, ARROW_RIGHT, LEFT_SHIFT,
    ESC, T, R, SPACE, LEFT_CTRL
  };

  OnlyMovable(KeyboardHandler)
  KeyboardHandler(MainWindow* window);
  KeyboardHandler::KeyState get_keystate(KeyboardHandler::InputKey key) const;
  void reset_state(InputKey key);
private:
  void key_callback(int key, int scancode, int action, int mods);
  // key, state
  std::map<InputKey, KeyState> m_keystate;
};
