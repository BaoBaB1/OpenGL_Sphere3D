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
#include "FrameBufferObject.hpp"
#include "macro.hpp"
#include "Camera.hpp"
#include "KeyboardHandler.hpp"
#include "CursorPositionHandler.hpp"
#include "MouseInputHandler.hpp"
#include "./ge/Cube.hpp"
#include "./ge/Icosahedron.hpp"
#include "./ge/Polyline.hpp"
#include "./ge/Pyramid.hpp"
#include "./ge/BezierCurve.hpp"
#include "./ge/Skybox.hpp"

static bool is_gui_opened();
static void setup_opengl();
static void setup_imgui(GLFWwindow*);
static std::string shading_mode_to_str(Object3D::ShadingMode mode);

SceneRenderer::SceneRenderer()
{
  m_window = std::make_unique<MainWindow>(800, 800, "OpenGLWindow");
  m_main_shader = std::make_unique<Shader>("./src/glsl/shader.vert", "./src/glsl/shader.frag");
  m_outlining_shader = std::make_unique<Shader>("./src/glsl/outlining.vert", "./src/glsl/outlining.frag");
  m_skybox_shader = std::make_unique<Shader>("./src/glsl/skybox.vert", "./src/glsl/skybox.frag");
  m_gpu_buffers = std::make_unique<GPUBuffers>();
  m_camera = std::make_unique<Camera>();
  m_camera->set_position(glm::vec3(-4.f, 2.f, 3.f));
  m_camera->look_at(glm::vec3(2.f, 0.5f, 0.5f));
  m_projection_mat = glm::mat4(1.f);
  m_projection_mat = glm::perspective(glm::radians(45.f), (float)m_window->height() / m_window->width(), 0.1f, 100.f);

  const int w = m_window->width();
  const int h = m_window->height();

  auto main_scene_fbo = std::make_unique<FrameBufferObject>(w, h);
  main_scene_fbo->bind();
  main_scene_fbo->attach_texture(w, h, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
  main_scene_fbo->attach_renderbuffer(GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
  main_scene_fbo->create_shader("./src/glsl/fbo_default_shader.vert", "./src/glsl/fbo_default_shader.frag");
  main_scene_fbo->unbind();
  m_fbos["main"] = std::move(main_scene_fbo);

  auto picking_fbo = std::make_unique<FrameBufferObject>(w, h);
  picking_fbo->bind();
  picking_fbo->attach_texture(w, h, GL_RGBA32F, GL_RGBA, GL_FLOAT);
  picking_fbo->attach_renderbuffer(GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT);
  picking_fbo->create_shader("./src/glsl/picking_fbo.vert", "./src/glsl/picking_fbo.frag");
  picking_fbo->unbind();
  m_fbos["picking"] = std::move(picking_fbo);
}

void SceneRenderer::render()
{
  GLFWwindow* gl_window = m_window->gl_window();
  ::setup_opengl();
  ::setup_imgui(gl_window);
  create_scene();

  for (const auto& [name, fbo] : m_fbos)
  {
    fbo->bind();
    assert(fbo->is_complete());
    fbo->unbind();
  }
  const auto& main_fbo = m_fbos.at("main");
  const auto& picking_fbo = m_fbos.at("picking");
  ScreenQuad screen_quad(main_fbo->texture().id());

  const std::string skybox_folder = ".\\.\\src\\textures\\skybox\\";
  std::array<std::string, 6> skybox_faces =
  { 
    skybox_folder + "right.jpg",
    skybox_folder + "left.jpg",
    skybox_folder + "top.jpg",
    skybox_folder + "bottom.jpg",
    skybox_folder + "front.jpg",
    skybox_folder + "back.jpg" 
  };
  Skybox skybox(std::move(skybox_faces));

  while (!glfwWindowShouldClose(gl_window))
  {
    glfwPollEvents();
    new_frame_update();
    handle_input();
    glPolygonMode(GL_FRONT_AND_BACK, m_polygon_mode);

    picking_fbo->bind();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    Shader& picking_shader = picking_fbo->shader();
    picking_shader.bind();
    render_scene(picking_shader, /*assignIndices*/true);
    picking_fbo->unbind();

    // render to a custom framebuffer
    main_fbo->bind();
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    m_main_shader->bind();
    // render scene before gui to make sure that imgui window always will be on top of drawn entities
    render_scene(*m_main_shader);
    // render skybox
    glDepthFunc(GL_LEQUAL);
    m_skybox_shader->bind();
    m_skybox_shader->set_matrix4f("viewMatrix", m_camera->view_matrix());
    m_skybox_shader->set_matrix4f("projectionMatrix", m_projection_mat);
    skybox.render(m_gpu_buffers.get());
    m_skybox_shader->unbind();
    glDepthFunc(GL_LESS);
    render_gui();
    main_fbo->unbind();

    // set GL_FILL mode because next we are rendering texture
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_DEPTH_TEST);
    main_fbo->shader().bind();
    screen_quad.render(m_gpu_buffers.get());

    glfwSwapBuffers(gl_window);
  }
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void SceneRenderer::render_scene(Shader& shader, bool assignIndices)
{
  shader.set_vec3("viewPos", m_camera->position());
  shader.set_matrix4f("viewMatrix", m_camera->view_matrix());
  shader.set_matrix4f("projectionMatrix", m_projection_mat);
  for (int i = 0; i < (int)m_drawables.size(); i++)
  {
    Object3D* pobj = m_drawables[i].get();
    IDrawable* pdrawable = static_cast<IDrawable*>(pobj);
    shader.set_matrix4f("modelMatrix", pobj->model_matrix());
    shader.set_bool("applyTexture", pobj->m_texture.has_value() && !pobj->m_texture->disabled());
    shader.set_bool("applyShading", pobj->m_shading_mode != Object3D::ShadingMode::NO_SHADING && !pobj->is_light_source());
    if (pobj->is_rotating())
    {
      pobj->rotate(pobj->m_rotation_angle, pobj->m_rotation_axis);
    }
    if (pobj->is_light_source())
    {
      // center in world space
      shader.set_vec3("lightPos", pobj->center() + glm::vec3(pobj->m_model_mat[3]));
      shader.set_vec3("lightColor", glm::vec3(1.f));
    }
    if (assignIndices)
    {
      shader.set_uint("objectIndex", i + 1);
    }
    // !assignIndices is a temp workaround to avoid crash when selecting same object twice,
    // because render outlining color to the picking FBO will overwrite assigned object index before
    // TODO: fix this
    // TODO: outlining for objects without surface (e.g. polylines)
    if (pobj->is_selected() && pobj->has_surface() && !assignIndices)
    {
      // first pass, fill stencil buffer
      glStencilFunc(GL_ALWAYS, 1, 0xFF);
      // enable writing to stencil buffer
      glStencilMask(0xFF);
      pdrawable->render(m_gpu_buffers.get());

      // second pass, discard fragments
      glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
      // disable writing to stencil buffer
      glStencilMask(0x00);
      glEnable(GL_DEPTH_TEST);
      // apply smooth shading to correctly draw outlining by moving all vertices along normal direction
      auto shading_mode = pobj->shading_mode();
      bool visible_normals = pobj->is_normals_visible();
      if (shading_mode != Object3D::ShadingMode::SMOOTH_SHADING)
        pobj->apply_shading(Object3D::ShadingMode::SMOOTH_SHADING);
      pobj->visible_normals(false);

      m_outlining_shader->bind();
      m_outlining_shader->set_matrix4f("viewMatrix", m_camera->view_matrix());
      m_outlining_shader->set_matrix4f("projectionMatrix", m_projection_mat);
      m_outlining_shader->set_matrix4f("modelMatrix", pobj->model_matrix());
      pdrawable->render(m_gpu_buffers.get());

      glStencilFunc(GL_ALWAYS, 0, 0xFF);
      glStencilMask(0xFF);

      // activate previous shader and set variables
      shader.bind();
      pobj->apply_shading(shading_mode);
      pobj->visible_normals(visible_normals);
    }
    else
    {
      pdrawable->render(m_gpu_buffers.get());
    }
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
  sphere->apply_shading(Object3D::ShadingMode::SMOOTH_SHADING);
  m_drawables.push_back(std::move(sphere));

  std::unique_ptr<Cube> c = std::make_unique<Cube>();
  c->translate(glm::vec3(0.5f, 0.f, 0.5f));
  c->scale(glm::vec3(0.5f));
  c->apply_shading(Object3D::ShadingMode::FLAT_SHADING);
  c->set_texture(".\\.\\src\\textures\\brick.jpg");
  m_drawables.push_back(std::move(c));

  std::unique_ptr<Cube> c2 = std::make_unique<Cube>();
  c2->translate(glm::vec3(2.f, 0.f, 0.5f));
  c2->set_color(glm::vec4(0.4f, 1.f, 0.4f, 1.f));
  c2->apply_shading(Object3D::ShadingMode::FLAT_SHADING);
  c2->visible_normals(true);
  m_drawables.push_back(std::move(c2));

  std::unique_ptr<Pyramid> pyr = std::make_unique<Pyramid>();
  pyr->translate(glm::vec3(0.5f, 0.f, 2.f));
  pyr->scale(glm::vec3(0.5f));
  pyr->set_color(glm::vec4(0.976f, 0.212f, 0.98f, 1.f));
  pyr->apply_shading(Object3D::ShadingMode::FLAT_SHADING);
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

void SceneRenderer::new_frame_update()
{
  ImGuiIO& io = ImGui::GetIO();
  m_camera->scale_speed(io.DeltaTime);

  double x, y;
  glfwGetCursorPos(m_window->gl_window(), &x, &y);
  // update virtual cursor pos to avoid camera jumps after cursor goes out of window or window regains focus,
  // because once cursor goes out of glfw window cursor callback is no longer triggered
  auto& h = m_window->input_handlers()[1];
  assert(h->type() == UserInputHandler::CURSOR_POSITION);
  static_cast<CursorPositionHandler*>(h.get())->update_current_pos(x, y);
}

void SceneRenderer::handle_input()
{
  for (const auto& phandler : m_window->input_handlers())
  {
    if (phandler->disabled())
      continue;
    switch (phandler->type())
    {
    case UserInputHandler::KEYBOARD:
    {
      KeyboardHandler* kh = static_cast<KeyboardHandler*>(phandler.get());
      if (kh->get_keystate(InputKey::W) == KeyboardHandler::PRESSED || kh->get_keystate(InputKey::ARROW_UP) == KeyboardHandler::PRESSED)
      {
        m_camera->move(Camera::Direction::FORWARD);
      }
      if (kh->get_keystate(InputKey::A) == KeyboardHandler::PRESSED || kh->get_keystate(InputKey::ARROW_LEFT) == KeyboardHandler::PRESSED)
      {
        m_camera->move(Camera::Direction::LEFT);
      }
      if (kh->get_keystate(InputKey::S) == KeyboardHandler::PRESSED || kh->get_keystate(InputKey::ARROW_DOWN) == KeyboardHandler::PRESSED)
      {
        m_camera->move(Camera::Direction::BACKWARD);
      }
      if (kh->get_keystate(InputKey::D) == KeyboardHandler::PRESSED || kh->get_keystate(InputKey::ARROW_RIGHT) == KeyboardHandler::PRESSED)
      {
        m_camera->move(Camera::Direction::RIGHT);
      }
      if (kh->get_keystate(InputKey::ESC) == KeyboardHandler::PRESSED)
      {
        for (auto& drawable : m_drawables)
        {
          drawable->select(false);
        }
      }
      KeyboardHandler::KeyState shift_state = kh->get_keystate(InputKey::LEFT_SHIFT);
      if (shift_state == KeyboardHandler::PRESSED)
      {
        m_camera->freeze();
        glfwSetInputMode(m_window->gl_window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      }
      else if (shift_state == KeyboardHandler::RELEASED)
      {
        m_camera->unfreeze();
        glfwSetInputMode(m_window->gl_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      }
    }
    break;
    case UserInputHandler::CURSOR_POSITION:
    {
      CursorPositionHandler* ch = static_cast<CursorPositionHandler*>(phandler.get());
      double x, y;
      ch->xy_offset(x, y);
      if (x != 0. || y != 0.)
        m_camera->add_to_yaw_and_pitch(x, y);
    }
    break;
    case UserInputHandler::MOUSE_INPUT:
    {
      MouseInputHandler* mh = static_cast<MouseInputHandler*>(phandler.get());
      if (mh->is_left_button_clicked())
      {
        int x, y;
        x = mh->x();
        y = mh->y();
        const auto& picking_fbo = m_fbos["picking"];
        picking_fbo->bind();
        glBindTexture(GL_TEXTURE_2D, picking_fbo->texture().id());
        float id[4] = {};
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, &id);
        if (id[0] != 0)
        {
          std::cout << "Pixel " << x << ',' << y << " object id = " << id[0] << '\n';
          int index = (int)id[0] - 1;
          const auto& clicked_obj = m_drawables[index];
          clicked_obj->select(true);
        }
        glReadBuffer(0);
        picking_fbo->unbind();
        mh->update_left_button_click_state();
      }
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
        glGetIntegerv(GL_POLYGON_MODE, &m_polygon_mode);
        if (m_polygon_mode == GL_LINE)
          m_polygon_mode = GL_FILL;
        else
          m_polygon_mode = GL_LINE;
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

          ImGui::SameLine();
          g_bools[4] = drawable->is_bbox_visible();
          if (ImGui::Checkbox("Show bounding box", &g_bools[4]))
            drawable->visible_bbox(g_bools[4]);

          // TODO: rewrite later as e.g. 2D Circle has surface but it won't be derived from Model class
          if (drawable->has_surface())
          {
            g_bools[3] = drawable->is_normals_visible();
            if (ImGui::Checkbox("Visible normals", &g_bools[3]))
              drawable->visible_normals(g_bools[3]);
            std::vector<std::pair<Object3D::ShadingMode, std::string>> modes(3);
            for (int i = 0; i < 3; i++)
            {
              Object3D::ShadingMode mode = static_cast<Object3D::ShadingMode>(i);
              modes[i] = std::make_pair(mode, ::shading_mode_to_str(mode));
            }
            std::string current_mode = ::shading_mode_to_str(drawable->shading_mode());
            if (ImGui::BeginCombo("Shading mode", current_mode.c_str()))
            {
              for (int i = 0; i < 3; i++)
              {
                if (ImGui::Selectable(modes[i].second.c_str(), &g_bools[4]))
                  drawable->apply_shading(modes[i].first);
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

void ScreenQuad::render(GPUBuffers* gpu_buffers)
{
  auto vao = gpu_buffers->vao;
  auto vbo = gpu_buffers->vbo;
  vao->bind();
  vbo->bind();
  vbo->set_data(quadVertices, sizeof(quadVertices));
  vao->link_attrib(0, 2, GL_FLOAT, sizeof(float) * 4, nullptr);
  vao->link_attrib(1, 2, GL_FLOAT, sizeof(float) * 4, (void*)(sizeof(float) * 2));
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_tex_id);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  vbo->unbind();
  vao->unbind();
}

static void setup_opengl()
{
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
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

static std::string shading_mode_to_str(Object3D::ShadingMode mode)
{
  switch (mode)
  {
  case Object3D::ShadingMode::SMOOTH_SHADING:
    return "Smooth shading";
  case Object3D::ShadingMode::FLAT_SHADING:
    return "Flat shading";
  case Object3D::ShadingMode::NO_SHADING:
    return "No shading";
  default:
    return "Unknown";
  }
}

static bool is_gui_opened()
{
  return g_bools[0];
}
