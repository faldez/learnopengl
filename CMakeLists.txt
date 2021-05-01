cmake_minimum_required(VERSION 3.17)
project(LearnOpenGL)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)

add_subdirectory(deps/glfw-3.3.3)
add_subdirectory(deps/glm)

set(EXECUTABLES_SRC 
    "src/1_Hello Window.cpp"
    "src/2_Hello Rectangle.cpp"
    "src/3_Hello Triangle Exercise 1.cpp"
    "src/3_Hello Triangle Exercise 2.cpp"
    "src/3_Hello Triangle Exercise 3.cpp"
    "src/4_Uniform.cpp"
    "src/5_Texture.cpp"
    "src/6_Transformation.cpp"
    "src/7_Coordinate Systems.cpp"
    "src/8_Camera.cpp"
    "src/9_Colors.cpp"
)

foreach(src ${EXECUTABLES_SRC})
    string( REPLACE "src/" "" execname ${src} )
    string( REPLACE ".cpp" "" execname ${execname} )
    string( REPLACE " " "" execname ${execname} )
    add_executable(${execname} ${src} src/glad.c src/stb_image.cpp src/shader.cpp)
    target_include_directories(${execname} PRIVATE include)
    target_link_libraries(${execname} glfw)
    target_link_libraries(${execname} glm)
    target_link_libraries(${execname} OpenGL::GL)
endforeach(src ${EXECUTABLES_SRC})