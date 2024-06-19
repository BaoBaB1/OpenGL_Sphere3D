#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "ImGuiFileDialog.h"

#include "Ui.hpp"
#include "SceneRenderer.hpp"
#include "MainWindow.hpp"
#include "ModelLoader.hpp"

#include <vector>
#include <string>

static std::string shading_mode_to_str(Object3D::ShadingMode mode);
static bool once = true;

Ui::Ui(SceneRenderer& scene, MainWindow* window) : m_scene(scene), m_window(window)
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  ImGui_ImplGlfw_InitForOpenGL(window->gl_window(), true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
  if (once)
  {
    ImGui_ImplOpenGL3_Init();
    once = false;
  }

  m_imgui_statesb.fill(false);
  m_imgui_statesf.fill(0.f);
}

Ui::~Ui()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void Ui::render()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  bool& st_gui_opened = m_imgui_statesb[0];
  bool& st_polygone_mode = m_imgui_statesb[1];
  bool& st_is_rotating = m_imgui_statesb[2];
  bool& st_is_bbox_visible = m_imgui_statesb[3];
  bool& st_is_normals_visible = m_imgui_statesb[4];
  bool& st_selected_shading_mode = m_imgui_statesb[5];
  bool& st_file_selection = m_imgui_statesb[6];

  float& st_translation_x = m_imgui_statesf[0];
  float& st_translation_y = m_imgui_statesf[1];
  float& st_translation_z = m_imgui_statesf[2];

  auto& scene = m_scene;

  // menu bar
  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      if (ImGui::MenuItem("Load", nullptr, &st_file_selection))
      {
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  // file selection mode
  if (st_file_selection)
  {
    // disable all handlers
    m_window->notify_all(false);
    glfwSetInputMode(m_window->gl_window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // create file dialog
    IGFD::FileDialogConfig config;
    config.path = ".";

    // display in the middle of the screen
    ImGui::SetNextWindowPos(ImVec2(m_window->width() * 0.5f, m_window->height() * 0.5f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImVec2 min_size(700, 300), max_size(1280, 650);

    auto& dlg = *ImGuiFileDialog::Instance();

    dlg.OpenDialog("FileLoaderDialog", "Select file to load", ".obj", config);
    if (dlg.Display("FileLoaderDialog", ImGuiWindowFlags_NoCollapse, min_size, max_size))
    {
      if (ImGuiFileDialog::Instance()->IsOk())
      {
        std::string selected_file = dlg.GetFilePathName();
        ModelLoader loader;
        std::optional<ComplexModel> m = loader.load(selected_file, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenBoundingBoxes /*| aiProcess_GenSmoothNormals*/);
        if (m)
        {
          scene.m_drawables.push_back(std::make_unique<ComplexModel>(std::move(*m)));
        }
      }

      // close
      st_file_selection = false;
      m_window->notify_all(true);
      glfwSetInputMode(m_window->gl_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      ImGuiFileDialog::Instance()->Close();
    }
  }

  // open/closed configuration menu
  if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_GraveAccent))
  {
    st_gui_opened = !st_gui_opened;
    const bool enable = !st_gui_opened;
    m_window->notify_all(enable);
    if (st_gui_opened)
      glfwSetInputMode(m_window->gl_window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else
      glfwSetInputMode(m_window->gl_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }

  if (st_gui_opened)
  {
    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (ImGui::CollapsingHeader("Configuration"))
    {
      if (ImGui::Checkbox("Don't fill polygons", &st_polygone_mode))
      {
        glGetIntegerv(GL_POLYGON_MODE, &scene.m_polygon_mode);
        if (scene.m_polygon_mode == GL_LINE)
          scene.m_polygon_mode = GL_FILL;
        else
          scene.m_polygon_mode = GL_LINE;
      }
    }
    if (ImGui::CollapsingHeader("Objects"))
    {
      size_t id = 1;
      for (const auto& drawable : scene.m_drawables)
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
          st_translation_x = drawable->m_translation.x;
          st_translation_y = drawable->m_translation.y;
          st_translation_z = drawable->m_translation.z;
          if (ImGui::SliderFloat("X", &st_translation_x, -10.0f, 10.0f))
            // prev = drawable->m_translation.x; curr = g_floats[0]; diff = curr - prev
            drawable->translate(glm::vec3(st_translation_x - drawable->m_translation.x, 0.f, 0.f));
          ImGui::SameLine();
          if (ImGui::SliderFloat("Y", &st_translation_y, -10.0f, 10.0f))
            drawable->translate(glm::vec3(0.f, st_translation_y - drawable->m_translation.y, 0.f));
          ImGui::SameLine();
          if (ImGui::SliderFloat("Z", &st_translation_z, -10.0f, 10.0f))
            drawable->translate(glm::vec3(0.f, 0.f, st_translation_z - drawable->m_translation.z));
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
          st_is_rotating = drawable->is_rotating();
          if (ImGui::Checkbox("Rotating", &st_is_rotating))
            drawable->rotating(st_is_rotating);
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
          st_is_bbox_visible = drawable->is_bbox_visible();
          if (ImGui::Checkbox("Show bounding box", &st_is_bbox_visible))
            drawable->visible_bbox(st_is_bbox_visible);

          // TODO: rewrite later as e.g. 2D Circle has surface but it won't be derived from Model class
          if (drawable->has_surface())
          {
            st_is_normals_visible = drawable->is_normals_visible();
            if (ImGui::Checkbox("Visible normals", &st_is_normals_visible))
              drawable->visible_normals(st_is_normals_visible);
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
                st_selected_shading_mode = (current_mode == modes[i].second);
                if (ImGui::Selectable(modes[i].second.c_str(), st_selected_shading_mode))
                  drawable->apply_shading(modes[i].first);
                if (st_selected_shading_mode)
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
