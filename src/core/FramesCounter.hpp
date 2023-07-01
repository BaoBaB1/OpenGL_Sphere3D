#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class FramesCounter {
public:
  FramesCounter(const char* window_title);
  double delta_time() { return m_delta_time; }
  void update_delta_time(double time);
  void update_fps(GLFWwindow* window, double time);
private:
  int m_frames;
  double m_last_fps_update;
  double m_last_frame;
  double m_delta_time;
  const char* m_window_title;
};