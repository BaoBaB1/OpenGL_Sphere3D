#pragma once

#include <array>

class SceneRenderer;
class MainWindow;
class Object3D;

class Ui
{
public:
  Ui(SceneRenderer& scene, MainWindow* window);
  ~Ui();
  void render();
private:
  void render_object_properties(Object3D& drawable);
  void render_xyz_markers(float offset_from_left, float width);
private:
  std::array<bool, 16> m_imgui_statesb;
  uint16_t m_guizmo_operation;
  MainWindow* m_window;
  SceneRenderer& m_scene;
};