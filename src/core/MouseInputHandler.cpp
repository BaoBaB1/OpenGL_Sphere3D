#include "MouseInputHandler.hpp"
#include "MainWindow.hpp"
#include "SceneRenderer.hpp"

MouseInputHandler::MouseInputHandler(MainWindow* window) : UserInputHandler(window, HandlerType::MOUSE_INPUT)
{
  auto lb_click_callback = [](GLFWwindow* window, int button, int action, int mods)
    {
      glfwSetWindowUserPointer(window, m_ptrs[HandlerType::MOUSE_INPUT]);
      static_cast<MouseInputHandler*>(glfwGetWindowUserPointer(window))->left_btn_click_callback(window, button, action, mods);
    };

  auto window_size_change_callback = [](GLFWwindow* window, int width, int height)
    {
      glfwSetWindowUserPointer(window, m_ptrs[HandlerType::MOUSE_INPUT]);
      static_cast<MouseInputHandler*>(glfwGetWindowUserPointer(window))->window_size_change_callback(window, width, height);
    };

  glfwSetWindowSizeCallback(m_window->gl_window(), window_size_change_callback);
  glfwSetMouseButtonCallback(m_window->gl_window(), lb_click_callback);
}

void MouseInputHandler::left_btn_click_callback(GLFWwindow* window, int button, int action, int mods)
{
  if (!disabled())
  {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
      double xd, yd;
      glfwGetCursorPos(window, &xd, &yd);
      auto& scene = SceneRenderer::instance();
      const int x = static_cast<int>(xd);
      const int y = scene.m_window->height() - static_cast<int>(yd);
      const auto& picking_fbo = scene.m_fbos["picking"];
      picking_fbo.bind();
      glBindTexture(GL_TEXTURE_2D, picking_fbo.texture()->id());
      float id[4] = {};
      glReadBuffer(GL_COLOR_ATTACHMENT0);
      glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, &id);
      if (id[0] != 0)
      {
        std::cout << "Pixel " << x << ',' << y << " object id = " << id[0] << '\n';
        int index = (int)id[0] - 1;
        const auto& clicked_obj = scene.m_drawables[index];
        clicked_obj->select(true);
      }
      glReadBuffer(0);
      picking_fbo.unbind();
    }
  }
}

void MouseInputHandler::window_size_change_callback(GLFWwindow* window, int width, int height)
{
  auto& s = Singleton<SceneRenderer>::instance();
  s.m_window->set_width(width);
  s.m_window->set_height(height);
  s.m_projection_mat = glm::mat4(1.f);
  s.m_projection_mat = glm::perspective(glm::radians(45.f), (float)width / height, 0.1f, 100.f);
  for (auto& [name, fbo] : s.m_fbos)
  {
    fbo.bind();
    // resize texture and render buffer
    fbo.attach_texture(width, height, fbo.texture()->internal_fmt(), fbo.texture()->format(), fbo.texture()->type());
    fbo.attach_renderbuffer(width, height, fbo.rb_internal_format(), fbo.rb_attachment());
    fbo.unbind();
  }
  glViewport(0, 0, width, height);
}
