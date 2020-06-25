cmake_minimum_required(VERSION 3.10)

set(CMAKE_SUPPRESS_REGENERATION true)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(sta)

add_subdirectory($ENV{GLFW3_DIR} ${PROJECT_SOURCE_DIR}/build-glfw3)
include_directories(%ENV{GLFW3_DIR}/include)

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

#get_target_property(GLFW3_INCLUDE_DIR glfw INTERFACE_INCLUDE_DIRECTORIES)
add_executable(sta ${contrib_sources} ${main_sources} ${main_headers})
target_link_libraries(sta glfw ${GLFW_LIBRARIES})
target_include_directories(sta PRIVATE contrib/json contrib/glad contrib/miniz contrib/imgui-1.76 $ENV{GLFW3_DIR}/include)

SET(CMAKE_DEFAULT_STARTUP_PROJECT sta)
