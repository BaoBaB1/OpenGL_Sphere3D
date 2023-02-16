
set(GLAD_SRC "${ROOT_DIR}/libs/glad/src")
set(SOURCES 
	MainWindow.hpp
	Icosahedron.hpp
	VertexArrayObject.hpp
	VertexBufferObject.hpp
	ElementBufferObject.hpp  
	Shader.hpp
	SceneRenderer.hpp
	MemoryProfiler.hpp
	macro.hpp
	Camera.hpp
	UserInputHandler.hpp
	Vertex.hpp
	Mesh.hpp
	Cube.hpp
	Model.hpp
	Face.hpp
	OpenGLMain.cpp
	MainWindow.cpp
	Icosahedron.cpp
	VertexArrayObject.cpp 
	VertexBufferObject.cpp 
	ElementBufferObject.cpp
	Shader.cpp
	SceneRenderer.cpp
	MemoryProfiler.cpp
	Camera.cpp
	UserInputHandler.cpp
	Vertex.cpp
	Mesh.cpp
	Cube.cpp
	Model.cpp
	Face.cpp
	${GLAD_SRC}/glad.c
)

file(GLOB_RECURSE RESOURCES
	"${ROOT_DIR}/default.frag"
	"${ROOT_DIR}/default.vert"
	)

add_executable(EngineOpenGL ${SOURCES} ${RESOURCES})
target_link_libraries(EngineOpenGL glfw)
set_target_properties(EngineOpenGL PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${ROOT_DIR}")
set_target_properties(glfw PROPERTIES FOLDER HiddenTargets)
source_group("Resources" FILES ${RESOURCES})
