include(FetchContent)

FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/BaoBaB1/GladGeneratedSources.git
  GIT_SHALLOW TRUE
  GIT_TAG main
)

FetchContent_MakeAvailable(glad)

add_library(glad STATIC ${glad_SOURCE_DIR}/glad46_core/src/glad.c)
target_include_directories(glad PUBLIC ${glad_SOURCE_DIR}/glad46_core/include)
