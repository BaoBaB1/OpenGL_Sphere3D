#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <cassert>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>

#include "SceneRenderer.hpp"
#include "MainWindow.hpp"
#include "Shader.hpp"
#include "VertexArrayObject.hpp"
#include "VertexBufferObject.hpp"
#include "ElementBufferObject.hpp"
#include "macro.hpp"
#include "Camera.hpp"
#include "KeyboardHandler.hpp"
#include "CursorHandler.hpp"
#include "./ge/Cube.hpp"
#include "./ge/Icosahedron.hpp"
#include "./ge/Polyline.hpp"
#include "./ge/Pyramid.hpp"
#include "./ge/BezierCurve.hpp"

static bool is_gui_opened();
static void setup_opengl();
static void setup_imgui(GLFWwindow*);
static std::string shading_mode_to_str(IShaderable::ShadingMode mode);

SceneRenderer::SceneRenderer()
{
  m_window = std::make_unique<MainWindow>(800, 800, "OpenGLWindow");
  m_shader = std::make_unique<Shader>("./src/glsl/shader.vert", "./src/glsl/shader.frag");
  m_gpu_buffers = std::make_unique<GPUBuffers>();
  m_camera = std::make_unique<Camera>();
  m_camera->set_position(glm::vec3(-4.f, 2.f, 3.f));
  m_camera->look_at(glm::vec3(2.f, 0.5f, 0.5f));
  m_projection_mat = glm::mat4(1.f);
  m_projection_mat = glm::perspective(glm::radians(45.f), (float)m_window->height() / m_window->width(), 0.1f, 100.f);
}

void SceneRenderer::render()
{
  GLFWwindow* gl_window = m_window->gl_window();
  ::setup_opengl();
  ::setup_imgui(gl_window);
  create_scene();
  m_shader->activate();
  while (!glfwWindowShouldClose(gl_window)) 
  {
    // color of the background
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    // Clean the back buffer and assign the new color to it
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwPollEvents();
    handle_input();
    // render scene before gui to make sure that imgui window always will be on top of drawn entities
    render_scene();
    render_gui();
    glfwSwapBuffers(gl_window);
  }
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void SceneRenderer::render_scene()
{
  for (const auto& obj : m_drawables) 
  {
    m_shader->set_vec3("viewPos", m_camera->position());
    m_shader->set_matrix4f("modelMatrix", obj->model_matrix());
    m_shader->set_matrix4f("viewMatrix", m_camera->view_matrix());
    m_shader->set_matrix4f("projectionMatrix", m_projection_mat);
    static_cast<IDrawable*>(&(*obj))->render(m_gpu_buffers.get(), m_shader.get());
  }
}

void SceneRenderer::create_scene()
{
  Vertex arr[6];
  arr[0].position = glm::vec3(0.f, 1.f, 0.f), arr[0].color = glm::vec4(0.f, 1.f, 0.f, 1.f);
  arr[1].position = glm::vec3(0.f, 0.f, 0.f), arr[1].color = glm::vec4(0.f, 1.f, 0.f, 1.f);
  arr[2].position = glm::vec3(0.f, 0.f, 0.f), arr[2].color = glm::vec4(1.f, 0.f, 0.f, 1.f);
  arr[3].position = glm::vec3(1.f, 0.f, 0.f), arr[3].color = glm::vec4(1.f, 0.f, 0.f, 1.f);
  arr[4].position = glm::vec3(0.f, 0.f, 0.f), arr[4].color = glm::vec4(0.f, 0.f, 1.f, 1.f);
  arr[5].position = glm::vec3(0.f, 0.f, 1.f), arr[5].color = glm::vec4(0.f, 0.f, 1.f, 1.f);
  std::unique_ptr<Polyline> origin = std::make_unique<Polyline>();
  for (int i = 0; i < 6; i++) {
    origin->add(arr[i]);
  }
  m_drawables.push_back(std::move(origin));

  std::unique_ptr<Icosahedron> sun = std::make_unique<Icosahedron>();
  sun->light_source(true);
  sun->translate(glm::vec3(0.f, 0.5f, 2.f));
  sun->set_color(glm::vec4(1.f, 1.f, 0.f, 1.f));
  sun->scale(glm::vec3(0.3f));
  sun->subdivide_triangles(4);
  sun->project_points_on_sphere();
  m_drawables.push_back(std::move(sun));
  
  std::unique_ptr<Icosahedron> sphere = std::make_unique<Icosahedron>();
  sphere->translate(glm::vec3(2.5f, 0.5f, 2.f));
  sphere->set_color(glm::vec4(1.f, 0.f, 0.f, 1.f));
  sphere->subdivide_triangles(4);
  sphere->project_points_on_sphere();
  sphere->scale(glm::vec3(0.3f));
  sphere->apply_shading(IShaderable::ShadingMode::SMOOTH_SHADING);
  m_drawables.push_back(std::move(sphere));
  
  std::unique_ptr<Cube> c = std::make_unique<Cube>();
  c->translate(glm::vec3(0.5f, 0.f, 0.5f));
  c->scale(glm::vec3(0.5f));
  c->apply_shading(IShaderable::ShadingMode::FLAT_SHADING);
  c->set_texture(".\\.\\src\\textures\\brick.jpg");
  m_drawables.push_back(std::move(c));

  std::unique_ptr<Cube> c2 = std::make_unique<Cube>();
  c2->translate(glm::vec3(2.f, 0.f, 0.5f));
  c2->set_color(glm::vec4(0.4f, 1.f, 0.4f, 1.f));
  c2->apply_shading(IShaderable::ShadingMode::FLAT_SHADING);
  c2->visible_normals(true);
  m_drawables.push_back(std::move(c2));

  std::unique_ptr<Pyramid> pyr = std::make_unique<Pyramid>();
  pyr->translate(glm::vec3(0.5f, 0.f,  2.f));
  pyr->scale(glm::vec3(0.5f));
  pyr->set_color(glm::vec4(0.976f, 0.212f, 0.98f, 1.f));
  pyr->apply_shading(IShaderable::ShadingMode::FLAT_SHADING);
  m_drawables.push_back(std::move(pyr));

  std::unique_ptr<BezierCurve> bc = std::make_unique<BezierCurve>(BezierCurve::Type::Quadratic);
  bc->set_start_point(Vertex());
  bc->set_end_point(Vertex(2.5f, 0.f, 0.f));
  bc->set_control_points({ Vertex(1.25f, 2.f, 0.f) });
  bc->set_color(glm::vec4(1.f, 0.f, 0.f, 1.f));
  m_drawables.push_back(std::move(bc));

  std::unique_ptr<BezierCurve> bc2 = std::make_unique<BezierCurve>(BezierCurve::Type::Cubic);
  bc2->set_start_point(Vertex());
  bc2->set_end_point(Vertex(0.f, 0.f, -2.5f));
  bc2->set_control_points({ Vertex(0.f, 2.f, -1.25f), Vertex {0.f, -2.f, -1.75} });
  bc2->set_color(glm::vec4(1.f, 0.f, 0.f, 1.f));
  m_drawables.push_back(std::move(bc2));
}

void SceneRenderer::handle_input()
{
  ImGuiIO& io = ImGui::GetIO();
  m_camera->scale_speed(io.DeltaTime);
  for (auto phandler: m_window->input_handlers()) 
  {
    if (phandler->disabled())
      continue;
    switch (phandler->type())
    {
    case InputType::KEYBOARD:
    {
      KeyboardHandler* kh = static_cast<KeyboardHandler*>(phandler);
      if (kh->key_state(InputKey::W) == GLFW_PRESS || kh->key_state(InputKey::ARROW_UP) == GLFW_PRESS)
        m_camera->move(Camera::Direction::FORWARD);
      if (kh->key_state(InputKey::A) == GLFW_PRESS || kh->key_state(InputKey::ARROW_LEFT) == GLFW_PRESS)
        m_camera->move(Camera::Direction::LEFT);
      if (kh->key_state(InputKey::S) == GLFW_PRESS || kh->key_state(InputKey::ARROW_DOWN) == GLFW_PRESS)
        m_camera->move(Camera::Direction::BACKWARD);
      if (kh->key_state(InputKey::D) == GLFW_PRESS || kh->key_state(InputKey::ARROW_RIGHT) == GLFW_PRESS)
        m_camera->move(Camera::Direction::RIGHT);
    }
    break;
    case InputType::CURSOR:
    {
      CursorHandler* ch = static_cast<CursorHandler*>(phandler);
      double x, y;
      ch->xy_offset(x, y);
      if (x != 0. || y != 0.)
        m_camera->add_to_yaw_and_pitch(x, y);
    }
    break;
    default:
      break;
    }
  }
}

static bool g_bools[16];
static float g_floats[16];

void SceneRenderer::render_gui()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  // open/closed configuration menu
  if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_GraveAccent))
  {
    g_bools[0] = !g_bools[0];
    // notify all input handlers
    m_window->notify_all(!g_bools[0]);
    if (::is_gui_opened())
      glfwSetInputMode(m_window->gl_window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else 
      glfwSetInputMode(m_window->gl_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
  if (::is_gui_opened())
  {
    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (ImGui::CollapsingHeader("Configuration"))
    {
      if (ImGui::Checkbox("Don't fill polygons", &g_bools[1]))
      {
        GLint polygonMode;
        glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
        if (polygonMode == GL_LINE)
          glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        else
          glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      }
    }
    if (ImGui::CollapsingHeader("Objects"))
    {
      size_t id = 1;
      for (const auto& drawable : m_drawables) 
      {
        std::string name = drawable->name() + std::to_string(id++);
        if (ImGui::TreeNode(name.c_str()))
        {
          float len = ImGui::GetWindowSize().x;
          // multiply by 3 because 'nesting' level is 3.
          float indent = ImGui::GetStyle().IndentSpacing * 3;
          indent += ImGui::CalcTextSize("X").x; 
          indent += ImGui::CalcTextSize("Y").x; 
          indent += ImGui::CalcTextSize("Z").x;
          len -= indent;
          ImGui::PushItemWidth(len / 3);

          ImGui::Separator();
          ImGui::Text("Translation");
          g_floats[0] = drawable->m_translation.x;
          g_floats[1] = drawable->m_translation.y;
          g_floats[2] = drawable->m_translation.z;
          if (ImGui::SliderFloat("X", &g_floats[0], -10.0f, 10.0f))
            // prev = drawable->m_translation.x; curr = g_floats[0]; diff = curr - prev
            drawable->translate(glm::vec3(g_floats[0] - drawable->m_translation.x, 0.f, 0.f));
          ImGui::SameLine();
          if (ImGui::SliderFloat("Y", &g_floats[1], -10.0f, 10.0f))
            drawable->translate(glm::vec3(0.f, g_floats[1] - drawable->m_translation.y, 0.f));
          ImGui::SameLine();
          if (ImGui::SliderFloat("Z", &g_floats[2], -10.0f, 10.0f))
            drawable->translate(glm::vec3(0.f, 0.f, g_floats[2] - drawable->m_translation.z));
          ImGui::Separator();

          ImGui::Text("Scale");
          if (ImGui::SliderFloat("X##2", &drawable->m_scale.x, 0.1f, 3.f))
            drawable->scale(drawable->m_scale);
          ImGui::SameLine();
          if (ImGui::SliderFloat("Y##2", &drawable->m_scale.y, 0.1f, 3.f))
            drawable->scale(drawable->m_scale);
          ImGui::SameLine();
          if (ImGui::SliderFloat("Z##2", &drawable->m_scale.z, 0.1f, 3.f))
            drawable->scale(drawable->m_scale);
          ImGui::Separator();

          ImGui::Text("Rotation");
          g_bools[2] = drawable->is_rotating();
          if (ImGui::Checkbox("Rotating", &g_bools[2]))
            drawable->rotating(g_bools[2]);
          if (ImGui::SliderAngle("Angle", &drawable->m_rotation_angle))
          {
            // if is_rotating == true, then rotation is made every frame in Object3D::render_geom()
            if (!drawable->is_rotating())
                drawable->rotate(drawable->m_rotation_angle, drawable->m_rotation_axis);
          }
          if (ImGui::SliderFloat("X##3", &drawable->m_rotation_axis.x, 0.f, 1.f))
          {
            if (!drawable->is_rotating())
              drawable->rotate(drawable->m_rotation_angle, drawable->m_rotation_axis);
          }
          ImGui::SameLine();
          if (ImGui::SliderFloat("Y##3", &drawable->m_rotation_axis.y, 0.f, 1.f))
          {
            if (!drawable->is_rotating())
              drawable->rotate(drawable->m_rotation_angle, drawable->m_rotation_axis);
          }
          ImGui::SameLine();
          if (ImGui::SliderFloat("Z##3", &drawable->m_rotation_axis.z, 0.f, 1.f))
          {
            if (!drawable->is_rotating())
              drawable->rotate(drawable->m_rotation_angle, drawable->m_rotation_axis);
          }
          ImGui::Separator();

          ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);
          ImGui::Text("Color");
          if (ImGui::ColorEdit4("Choose color", &drawable->m_color.x))
            drawable->set_color(drawable->m_color);
          ImGui::Separator();

          // TODO: rewrite later as e.g. 2D Circle has surface but it won't be derived from Model class
          if (drawable->has_surface())
          {
            Model* model = static_cast<Model*>(drawable.get());
            g_bools[3] = model->is_normals_visible();
            if (ImGui::Checkbox("Visible normals", &g_bools[3]))
              model->visible_normals(g_bools[3]);
            ImGui::SameLine();
            g_bools[4] = model->is_bbox_visible();
            if (ImGui::Checkbox("Draw bounding box", &g_bools[4]))
              model->visible_bbox(g_bools[4]);
            std::vector<std::pair<IShaderable::ShadingMode, std::string>> modes(3);
            for (int i = 1; i <= 3; i++)
            {
              IShaderable::ShadingMode mode = static_cast<IShaderable::ShadingMode>(i);
              modes[i - 1] = std::make_pair(mode, ::shading_mode_to_str(mode));
            }
            std::string current_mode = ::shading_mode_to_str(model->shading_mode());
            if (ImGui::BeginCombo("Shading mode", current_mode.c_str()))
            {
              for (int i = 0; i < 3; i++)
              {
                if (ImGui::Selectable(modes[i].second.c_str(), &g_bools[4]))
                  model->apply_shading(modes[i].first);
                if (g_bools[4])
                  ImGui::SetItemDefaultFocus();
              }
              ImGui::EndCombo();
            }
            ImGui::Separator();
          }
          ImGui::TreePop();
        }
      }
    }
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
  }
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

static void setup_opengl()
{
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void setup_imgui(GLFWwindow* window)
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  ImGui_ImplGlfw_InitForOpenGL(window, true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
  ImGui_ImplOpenGL3_Init();
}

static std::string shading_mode_to_str(IShaderable::ShadingMode mode)
{
  switch (mode)
  {
  case IShaderable::ShadingMode::SMOOTH_SHADING:
    return "Smooth shading";
  case IShaderable::ShadingMode::FLAT_SHADING:
    return "Flat shading";
  case IShaderable::ShadingMode::NO_SHADING:
    return "No shading";
  default:
    return "Unknown";
  }
}

static bool is_gui_opened()
{
  return g_bools[0];
}
