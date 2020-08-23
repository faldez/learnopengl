cmake_minimum_required(VERSION 3.17)
project(LearnOpenGL)

set(CMAKE_CXX_STANDARD 14)

add_executable(LearnOpenGL src/main.cpp src/glad.c)
find_package(OpenGL REQUIRED)

add_subdirectory(deps/glfw-3.3.2)

target_include_directories(LearnOpenGL PRIVATE include)

target_link_libraries(LearnOpenGL glfw)
target_link_libraries(LearnOpenGL OpenGL::GL)