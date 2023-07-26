#pragma once

#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <GLFW/glfw3.h>
#include <map>
#include "./utils/IObserver.hpp"

class MainWindow;

using namespace OpenGLEngineUtils;

enum InputType
{
  KEYBOARD = 1, 
  CURSOR
};

class UserInputHandler : public IObserver
{
public:
  virtual void enable() { m_disabled = false; }
  virtual void disable() { m_disabled = true; }
  void notify(bool _enable) override;
  bool disabled() const { return m_disabled; }
  InputType type() const { return m_type; }
protected:
  UserInputHandler(MainWindow* window, InputType input_type);
protected:
  InputType m_type;
  MainWindow* m_window;
  bool m_disabled;
  static std::map<InputType, void*> m_ptrs; // for correct cast in callbacks for glfwSetWindowUserPointer
};
