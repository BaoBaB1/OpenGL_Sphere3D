cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 11)
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
project(EngineOpenGL)

include_directories("libs/glfw/include")
include_directories("libs/glad/include")
include_directories("libs/glm")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory("${ROOT_DIR}/libs/glfw")
add_subdirectory("${ROOT_DIR}/src")
