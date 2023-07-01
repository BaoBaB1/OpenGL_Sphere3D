#include "FramesCounter.hpp"
#include <sstream>

FramesCounter::FramesCounter(const char* window_title) {
  m_window_title = window_title;
  m_last_fps_update = glfwGetTime();
  m_last_frame = m_last_fps_update;
  m_delta_time = 0.01;
  m_frames = 0;
}

void FramesCounter::update_delta_time(double time) {
  // time between 2 frames
  m_delta_time = time - m_last_frame;
  m_last_frame = time;
}

void FramesCounter::update_fps(GLFWwindow* window, double time) {
  if (time - m_last_fps_update > 1.) {
    std::stringstream ss;
    double fps = (m_frames) / (time - m_last_fps_update);
    ss << m_window_title << " [FPS = " << fps << " ]";
    glfwSetWindowTitle(window, ss.str().c_str());
    m_frames = 0;
    m_last_fps_update = time;
  }
  else {
    ++m_frames;
  }
}
