cmake_policy(VERSION 3.21)
cmake_minimum_required(VERSION 3.16...3.28)
project(MarchingCubes VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Get dependencies
include(FetchContent)

message(STATUS "Fetching GLFW...")
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

message(STATUS "Fetching GLM...")
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG master
)
FetchContent_MakeAvailable(glm)

find_package(OpenGL REQUIRED)



file(GLOB_RECURSE SRC_FILES "src/*.cpp")

add_subdirectory(external/FastNoise2)

add_executable(MarchingCubes ${SRC_FILES} external/glad/src/glad.c)
set(CMAKE_CXX_FLAGS "-O3")

target_include_directories(MarchingCubes PRIVATE
  src
  include
  external/glad/include
)

target_link_libraries(MarchingCubes PRIVATE
  glfw
  OpenGL::GL
  glm::glm
  FastNoise
)

if (WIN32)
  target_link_libraries(MarchingCubes PRIVATE OpenGL32)
endif()

add_custom_target(run
  COMMAND MarchingCubes
  DEPENDS MarchingCubes
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

install(TARGETS MarchingCubes DESTINATION bin)
