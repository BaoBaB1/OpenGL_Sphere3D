#pragma once

#include <iostream>
#include <string>

#define DEBUG(x) do { std::cerr << x; } while (0)

#define DEBUG_OPENGL(funcname)                                                    \
  {                                                                               \
    GLenum error = glGetError();                                                  \
    if (error != GL_NO_ERROR)                                                     \
    {                                                                             \
      std::string s(funcname);                                                    \
      if (s.empty())                                                              \
      {                                                                           \
        std::cerr << "OpenGL error " << error << std::endl;                       \
      }                                                                           \
      else                                                                        \
      {                                                                           \
        std::cerr << "OpenGL error in " << funcname << " " << error << std::endl; \
      }                                                                           \
    } \
  }
