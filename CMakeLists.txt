cmake_minimum_required(VERSION 3.10)

set(CMAKE_SUPPRESS_REGENERATION true)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(sta)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)

FILE(GLOB contrib_sources
  contrib/**/*.cpp
  contrib/**/*.c)

FILE(GLOB main_sources
  src/*.cpp
  src/*.c)

FILE(GLOB main_headers
  src/*.hpp
  src/*.h)

source_group(TREE ${PROJECT_SOURCE_DIR}/contrib PREFIX "Source Files\\contrib" FILES ${contrib_sources})
source_group(TREE ${PROJECT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${main_sources})
source_group(TREE ${PROJECT_SOURCE_DIR}/src PREFIX "Header Files" FILES ${main_headers})

link_directories($ENV{GLFW3_DIR}/../..)
add_executable(sta ${contrib_sources} ${main_sources} ${main_headers})
target_include_directories(sta PRIVATE contrib/json contrib/glad contrib/miniz contrib/imgui-1.76 $ENV{GLFW3_DIR}/../../../include)
target_link_libraries(sta PRIVATE glfw3)
