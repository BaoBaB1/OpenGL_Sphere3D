#pragma once

#include <vector>
#include <memory>
#include <map>
#include "Shader.hpp"
#include "Camera.hpp"
#include "FrameBufferObject.hpp"
#include "KeyboardHandler.hpp"
#include "GPUBuffers.hpp"
#include "./ge/Object3D.hpp"

class MainWindow;

class SceneRenderer {
  using InputKey = KeyboardHandler::InputKey;
public:
  SceneRenderer();
  void render();
private:
  struct ScreenQuad
  {
    ScreenQuad();
    void render();
    VertexArrayObject vao;
    VertexBufferObject vbo;
  };
  void handle_input();
  void render_scene(Shader& shader, bool assignIndices = false);
  void create_scene();
  void render_gui();
private:
  std::unique_ptr<MainWindow> m_window;
  std::unique_ptr<Shader> m_main_shader;
  std::unique_ptr<Shader> m_outlining_shader;
  std::unique_ptr<Camera> m_camera;
  std::unique_ptr<GPUBuffers> m_gpu_buffers;
  std::map<std::string, std::unique_ptr<FrameBufferObject>> m_fbos;
  std::vector<std::unique_ptr<Object3D>> m_drawables;
  glm::mat4 m_projection_mat;
  GLint m_polygon_mode = GL_FILL;
};
