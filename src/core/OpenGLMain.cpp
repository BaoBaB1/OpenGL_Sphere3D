#include "./core/SceneRenderer.hpp"

int main() 
{
  auto& scene = SceneRenderer::instance();
  scene.render();
  return 0;
}
