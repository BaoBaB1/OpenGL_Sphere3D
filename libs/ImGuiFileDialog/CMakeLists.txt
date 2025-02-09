include(FetchContent)

FetchContent_Declare(
  ImGuiFileDialog
  GIT_REPOSITORY https://github.com/aiekick/ImGuiFileDialog.git
  GIT_SHALLOW TRUE
  GIT_TAG        v0.6.7
)

FetchContent_MakeAvailable(ImGuiFileDialog)

target_link_libraries(ImGuiFileDialog PUBLIC imgui)
