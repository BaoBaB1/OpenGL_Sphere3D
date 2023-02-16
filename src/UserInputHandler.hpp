#pragma once

#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <GLFW/glfw3.h>
#include <map>

class MainWindow;

enum InputType {
	KEYBOARD = 1, 
	CURSOR
};

class UserInputHandler {
public:
	UserInputHandler(MainWindow* window, InputType input_type);
	virtual ~UserInputHandler();
	InputType type() const { return m_type; }
protected:
	InputType m_type;
	MainWindow* m_window;
	static std::map<InputType, void*> m_ptrs; // for correct cast in callbacks for glfwSetWindowUserPointer
};

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
	~KeyboardHandler();
	int key_state(InputKey key) const;
private:
	void key_callback(int key, int scancode, int action, int mods);
	// key, state
	std::map<int, int> m_keys;
};

class CursorHandler : public UserInputHandler {
public:
	CursorHandler(MainWindow* window);
	~CursorHandler();
	void xy_offset(double& x, double& y);
private:
	void cursor_callback(double xpos, double ypos);
	double m_prev_pos[2] = {};
	double m_cur_pos[2] = {};
	bool m_changed;
};
