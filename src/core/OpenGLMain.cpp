#include "./core/SceneRenderer.hpp"
#include "./utils/Singleton.hpp"

int main() 
{
  using namespace OpenGLEngineUtils;
  auto& scene = Singleton<SceneRenderer>::instance();
  scene.render();
  return 0;
}
