cmake_minimum_required(VERSION 3.8)
set (CMAKE_CXX_STANDARD 11)
project(OpenGLSphere3D)
include_directories("Libraries/glfw-3.3.8/include")
include_directories("Libraries/glad/include")
include_directories("Libraries/glm")
set(GLAD_SRC "Libraries/glad/src")
set(SOURCES 
	MainWindow.h
	Icosahedron.h
	GeometryPrimitives.h
	VertexArrayObject.h
	VertexBufferObject.h
	ElementBufferObject.h  
	Shader.h
	SceneRenderer.h
	MemoryProfiler.h
	macro.h
	OpenGLMain.cpp
	MainWindow.cpp
	Icosahedron.cpp
	GeometryPrimitives.cpp
	VertexArrayObject.cpp 
	VertexBufferObject.cpp 
	ElementBufferObject.cpp
	Shader.cpp
	SceneRenderer.cpp
	MemoryProfiler.cpp
	${GLAD_SRC}/glad.c)

file(GLOB_RECURSE RESOURCES
	default.frag
	default.vert
	)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_executable(OpenGLSphere3D ${SOURCES} ${RESOURCES})
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Libraries/glfw")
target_link_libraries(OpenGLSphere3D glfw)
source_group("Resources" FILES ${RESOURCES})
