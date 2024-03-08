#include "MainWindow.hpp"
#include "KeyboardHandler.hpp"
#include "CursorPositionHandler.hpp"
#include "MouseInputHandler.hpp"
#include "macro.hpp"

MainWindow::MainWindow(int width, int height, const char* title) :
  m_width(width), m_height(height), m_title(title) 
{
  glfwInit();
  // Tell GLFW what version of OpenGL we are using 
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
  // Tell GLFW we are using the CORE profile (only modern functions)
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (m_window == nullptr) {
    DEBUG("Failed to create GLFW window" << std::endl);
    glfwTerminate();
  }
  glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPos(m_window, m_width / 2., m_height / 2.);
  m_input_handlers.emplace_back(new KeyboardHandler(this));
  m_input_handlers.emplace_back(new CursorPositionHandler(this));
  m_input_handlers.emplace_back(new MouseInputHandler(this));
  m_observers.push_back(m_input_handlers[0]);
  m_observers.push_back(m_input_handlers[1]);
  m_observers.push_back(m_input_handlers[2]);
  glfwMakeContextCurrent(m_window);
  gladLoadGL();
  glViewport(0, 0, m_width, m_height);
}

void MainWindow::remove(IObserver* observer)
{
  auto it = std::find(m_observers.begin(), m_observers.end(), observer);
  if (it != m_observers.end())
    delete *it;
}

void MainWindow::add(IObserver* observer)
{
  m_observers.push_back(observer);
}

void MainWindow::notify(IObserver* observer, bool enable)
{
  observer->notify(enable);
}

void MainWindow::notify_all(bool enable)
{
  for (auto pobs : m_observers)
    pobs->notify(enable);
}

MainWindow::~MainWindow() 
{
  // handlers are observers themselves (added in constructor)
  for (auto pobs : m_observers)
    delete pobs;
  glfwDestroyWindow(m_window);
  glfwTerminate();
}
