cmake_minimum_required(VERSION 3.13.0)
# latest imgui requires at least c++17
set(CMAKE_CXX_STANDARD 17)
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PROJECT_NAME "OpenGLEngine")
project(${PROJECT_NAME})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# glfw/glad
include_directories(libs/glfw/include)
include_directories(libs/glad/include)
add_subdirectory(libs/glfw)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
target_sources(glfw PRIVATE libs/glad/src/glad.c)
set_target_properties(glfw PROPERTIES FOLDER ThirdParty)
# --------glfw/glad--------


# glm
include_directories(libs/glm)
# --------glm--------


# stb_image
include_directories(libs/stb)
# --------stb_image--------


# imgui
set(IMGUI_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui)
file(GLOB IMGUI_SRC
	${IMGUI_PATH}/*.h
	${IMGUI_PATH}/*.cpp
	)
add_library("imgui" STATIC ${IMGUI_SRC})
target_include_directories("imgui" PUBLIC ${IMGUI_PATH})
set_target_properties("imgui" PROPERTIES FOLDER ThirdParty)
# --------imgui--------


# ImGuiFileDialog
set(FILE_DIALOG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGuiFileDialog)
file(GLOB FILE_DIALOG_SRC
	${FILE_DIALOG_PATH}/*.h
	${FILE_DIALOG_PATH}/*.cpp
	)
add_library("ImGuiFileDialog" STATIC ${FILE_DIALOG_SRC})
target_include_directories("ImGuiFileDialog" PUBLIC ${FILE_DIALOG_PATH} ${IMGUI_PATH})
set_target_properties("ImGuiFileDialog" PROPERTIES FOLDER ThirdParty)
# --------ImGuiFileDialog--------


# assimp
include_directories(libs/assimp/include)
add_subdirectory(libs/assimp)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set_target_properties(assimp PROPERTIES FOLDER ThirdParty)
# ------------assimp------------


# OpenGLEngine sources
include_directories(src)
file(GLOB SHADERS 
	src/glsl/*.frag
	src/glsl/*.vert
	)
file(GLOB_RECURSE SOURCES
	"src/*.h"
	"src/*.hpp"
	"src/*.cpp"
	"libs/stb/stb_image.cpp"
	)

add_executable(${PROJECT_NAME} "src/core/OpenGLMain.cpp" ${SOURCES} ${SHADERS})
target_link_libraries(${PROJECT_NAME} PUBLIC glfw imgui assimp ImGuiFileDialog)
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${ROOT_DIR})
source_group("Resources" FILES ${SHADERS})
