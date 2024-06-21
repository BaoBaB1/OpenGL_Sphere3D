#pragma once

#include <vector>
#include <memory>
#include <map>
#include "Shader.hpp"
#include "Camera.hpp"
#include "FrameBufferObject.hpp"
#include "GPUBuffers.hpp"
#include "MainWindow.hpp"
#include "./utils/Singleton.hpp"
#include "./ge/Object3D.hpp"

class MouseInputHandler;
class CursorPositionHandler;
class Ui;

class SceneRenderer
{
public:
  static SceneRenderer& instance() { return Singleton<SceneRenderer>::instance(); }
  ~SceneRenderer();
  void render();
private:
  SceneRenderer();
  void handle_input();
  void render_scene(Shader& shader, bool assignIndices = false);
  void create_scene();
  void new_frame_update();
  friend class MouseInputHandler;
  friend class CursorPositionHandler;
  friend class Singleton<SceneRenderer>;
  friend class Ui;
private:
  std::vector<std::unique_ptr<Object3D>> m_drawables;
  std::unique_ptr<MainWindow> m_window;
  std::unique_ptr<GPUBuffers> m_gpu_buffers;
  std::unique_ptr<Ui> m_ui;
  Camera m_camera;
  std::map<std::string, FrameBufferObject> m_fbos;
  glm::mat4 m_projection_mat;
  GLint m_polygon_mode = GL_FILL;
};

struct ScreenQuad : IDrawable
{
  ScreenQuad(GLuint tex_id) : m_tex_id(tex_id) {}
  void render(GPUBuffers*);
  bool has_surface() const override { return false; }
  std::string name() const override { return "ScreenQuad"; }
  GLuint m_tex_id;

  static constexpr float quadVertices[] =
  { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
  };
};
