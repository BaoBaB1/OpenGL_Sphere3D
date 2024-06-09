#include "./core/SceneRenderer.hpp"

int main() 
{
  using namespace OpenGLEngineUtils;
  auto& scene = SceneRenderer::instance();
  scene.render();
  return 0;
}
