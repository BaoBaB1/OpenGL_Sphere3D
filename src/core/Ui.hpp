#pragma once

#include <array>

class SceneRenderer;
class MainWindow;

class Ui
{
public:
  Ui(SceneRenderer& scene, MainWindow* window);
  ~Ui();
  void render();
  void on_open_file();
private:
  std::array<bool, 16> m_imgui_statesb;
  std::array<float, 16> m_imgui_statesf;
  MainWindow* m_window;
  SceneRenderer& m_scene;
};