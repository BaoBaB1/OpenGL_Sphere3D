include(FetchContent)

FetchContent_Declare(
  ImGuizmo
  GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
  GIT_SHALLOW TRUE
  GIT_TAG        master
)

FetchContent_MakeAvailable(ImGuizmo)

# add_library(ImGuizmo imguizmo_SOURCE_DIR) doesn't work for some reason, so grab sources manually
file(GLOB SOURCES "${imguizmo_SOURCE_DIR}/*.h" "${imguizmo_SOURCE_DIR}/*.cpp")
add_library(ImGuizmo STATIC ${SOURCES})
target_include_directories(ImGuizmo PUBLIC ${imguizmo_SOURCE_DIR})
target_link_libraries(ImGuizmo PUBLIC imgui)
