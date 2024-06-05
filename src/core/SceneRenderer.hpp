#pragma once

#include <vector>
#include <memory>
#include <map>
#include "Shader.hpp"
#include "Camera.hpp"
#include "FrameBufferObject.hpp"
#include "GPUBuffers.hpp"
#include "MainWindow.hpp"
#include "./ge/Object3D.hpp"

class SceneRenderer 
{
public:
  SceneRenderer();
  void render();
private:
  void handle_input();
  void render_scene(Shader& shader, bool assignIndices = false);
  void create_scene();
  void render_gui();
  void new_frame_update();
private:
  std::vector<std::unique_ptr<Object3D>> m_drawables;
  MainWindow m_window;
  // make sure that OpenGL objects are created after glad/glfw setup in MainWindow
  GPUBuffers m_gpu_buffers;
  Shader m_main_shader;
  Shader m_outlining_shader;
  Shader m_skybox_shader;
  Shader m_fbo_default_shader;
  Shader m_picking_shader;
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
