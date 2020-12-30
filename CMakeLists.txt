cmake_minimum_required(VERSION 3.17)
project(LearnOpenGL)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)

add_subdirectory(deps/glfw-3.3.2)
add_subdirectory(deps/glm)

set(EXECUTABLES_SRC 
    "src/Hello Window.cpp"
    "src/Hello Rectangle.cpp"
    "src/Hello Triangle Exercise 1.cpp"
    "src/Hello Triangle Exercise 2.cpp"
    "src/Hello Triangle Exercise 3.cpp"
    "src/Uniform.cpp"
    "src/Texture.cpp"
    "src/Transformation.cpp"
    "src/Coordinate Systems.cpp"
)

foreach(src ${EXECUTABLES_SRC})
    string( REPLACE "src/" "" execname ${src} )
    string( REPLACE ".cpp" "" execname ${execname} )
    string( REPLACE " " "" execname ${execname} )
    add_executable(${execname} ${src} src/glad.c src/stb_image.cpp)
    target_include_directories(${execname} PRIVATE include)
    target_link_libraries(${execname} glfw)
    target_link_libraries(${execname} glm)
    target_link_libraries(${execname} OpenGL::GL)
endforeach(src ${EXECUTABLES_SRC})