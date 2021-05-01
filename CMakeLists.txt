cmake_minimum_required(VERSION 3.17)
project(LearnOpenGL)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)

add_subdirectory(deps/glfw-3.3.3)
add_subdirectory(deps/glm)

set(CHAPTERS 
    "hello_Window"
    "hello_triangle"
    "uniform"
    "texture"
    "transformation"
    "coordinate_systems"
    "camera"
    "colors"
)

set(hello_window
    "hello_window.cpp"
)

set(hello_triangle
    "hello_rectangle.cpp"
    "hello_triangle_exercise_1.cpp"
    "hello_triangle_exercise_2.cpp"
    "hello_triangle_exercise_3.cpp"
)

set(uniform
    "uniform.cpp"
)

set(texture
    "texture.cpp"
)

set(transformation
    "transformation.cpp"
)

set(coordinate_systems
    "coordinate_systems.cpp"
)

set(camera
    "camera.cpp"
)

set(colors
    "colors.cpp"
)

foreach(chapter ${CHAPTERS})
    foreach(src ${${chapter}})
        string( REPLACE ".cpp" "" execname ${src} )
        add_executable(${execname} src/${chapter}/${src} src/glad.c src/stb_image.cpp src/shader.cpp)
        target_include_directories(${execname} PRIVATE include)
        target_link_libraries(${execname} glfw)
        target_link_libraries(${execname} glm)
        target_link_libraries(${execname} OpenGL::GL)
        file(GLOB SHADERS
            "src/${chapter}/*.vert"
            "src/${chapter}/*.frag"
        )
        file(COPY ${SHADERS} DESTINATION ${CMAKE_SOURCE_DIR}/build)
    endforeach(src ${${chapter}})
endforeach(chapter ${CHAPTERS})