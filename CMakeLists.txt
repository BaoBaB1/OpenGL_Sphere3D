cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 14)
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PROJECT_NAME "OpenGLEngine")

project(${PROJECT_NAME})

include_directories("libs/glfw/include")
include_directories("libs/glad/include")
include_directories("libs/glm")
include_directories("libs/stb")
include_directories("src")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory("libs/glfw")

file(GLOB_RECURSE SHADERS
	"src/glsl/fragment_shader"
	"src/glsl/vertex_shader"
	)
	
file(GLOB_RECURSE SOURCES
	"libs/glad/glad.c"
	"libs/stb/stb_image.cpp"
	"src/*.h"
	"src/*hpp"
	"src/*cpp"
	)

add_executable(${PROJECT_NAME} "src/core/OpenGLMain.cpp" ${SOURCES} ${SHADERS})
target_link_libraries(${PROJECT_NAME} glfw)
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${ROOT_DIR})
source_group("Resources" FILES ${SHADERS})

