cmake_minimum_required(VERSION 3.10)

set(CMAKE_SUPPRESS_REGENERATION true)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(sta)

link_directories($ENV{GLFW3_LIB_DIR})

find_package(OpenGL REQUIRED)

FILE(GLOB_RECURSE contrib_sources
  contrib/**/*.cpp
  contrib/**/*.c)

FILE(GLOB_RECURSE main_sources
  src/*.cpp
  src/*.c)

FILE(GLOB_RECURSE main_headers
  src/*.hpp
  src/*.h)

#source_group(TREE ${PROJECT_SOURCE_DIR}/contrib PREFIX "Source Files\\contrib" FILES ${contrib_sources})
#source_group(TREE ${PROJECT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${main_sources})
#source_group(TREE ${PROJECT_SOURCE_DIR}/src PREFIX "Header Files" FILES ${main_headers})

add_executable(sta ${contrib_sources} ${main_sources} ${main_headers})
target_link_libraries(sta glfw3)
target_include_directories(sta PRIVATE src contrib/json contrib/glad contrib/miniz contrib/imgui-1.76 $ENV{GLFW3_DIR}/include)

set_property(GLOBAL PROPERTY VS_STARTUP_PROJECT sta)
