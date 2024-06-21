#include "ShaderStorage.hpp"
#include <vector>
#include <utility>
#include <string>

namespace GlobalState
{
  std::map<ShaderStorage::ShaderType, Shader> ShaderStorage::m_shaders;

  void ShaderStorage::init()
  {
    static bool once = true;
    if (once)
    {
      std::vector<std::pair<std::string, std::string>> sources = {
        {"./src/glsl/shader.vert", "./src/glsl/shader.frag"},
        {"./src/glsl/outlining.vert", "./src/glsl/outlining.frag"},
        {"./src/glsl/skybox.vert", "./src/glsl/skybox.frag"},
        {"./src/glsl/fbo_default_shader.vert", "./src/glsl/fbo_default_shader.frag"},
        {"./src/glsl/picking_fbo.vert", "./src/glsl/picking_fbo.frag"},
        {"./src/glsl/lines.vert", "./src/glsl/lines.frag"}
      };
      for (int i = 0; i < ShaderStorage::LAST_ITEM; i++)
      {
        Shader s;
        s.load(sources[i].first.data(), sources[i].second.data());
        m_shaders[static_cast<ShaderType>(i)] = std::move(s);
      }
      once = false;
    }
  }

  Shader* ShaderStorage::get(unsigned int id)
  {
    for (auto& item : m_shaders)
    {
      if (item.second.id() == id)
        return &(item.second);
    }
    return nullptr;
  }
};