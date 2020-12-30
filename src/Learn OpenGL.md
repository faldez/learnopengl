# Learn OpenGL
## Introduction
### Core-profile vs Immediate mode
* Core-profile
	* forced modern practices
	* flexible and efficient
	* more difficult
* Immediate mode 
	* easy to use
	* most of the functionality hidden 
	* (fixed function pipeline)
	* extremely inefficient
	* deprecated in 3.2

### Extensions
* manufacture's implementation
* developer has to query

```c++
if(GL_ARB_extension_name)
{
    // Do cool new and modern stuff supported by hardware
}
else
{
    // Extension not supported: do it the old way
}
```

### State Machine
* OpenGL is a large state machine
* The state is OpenGL context
* When using OpenGL
	* change its state by setting some option
	* manipulate buffers
	* render using current context
* There are state-change function that change context and state-using function that perform operations

### Objects
* represent a subset of OpenGL state

```c
struct object_name {
	float	option1;
	int		option2;
	char[]	name;
};
```

* to use object, generally

```c
// The state of OpenGL
struct OpenGL_Context {
	...
	object_name* object_Window_target;
	...
};
```

```c
// create object
unsigned int objectId = 0;
glGenObject(1, &objectId);
// bin/assign object to context
glBindObject(GL_WINDOW_TARGET, objectId);
// set options of object currently bount to GL_WINDOW_TARGET
glSetObjectOption(GL_WINDOW_TARGET, GL_OPTION_WINDOW_WIDTH, 800);
glSetObjectOption(GL_WINDOW_TARGET, GL_OPTION_WINDOW_HEIGHT, 600);
// set context target back to default
glBindObject(GL_WINDOW_TARGET, 0);
```

==We can define more than one object==

## Creating a Window
### GLFW
GLFW gives bare necessities required to render goodies to the scene

In macOS in order to use GLFW, I download glow source code, put it in subdirectory  `deps`,  and use this CMake config

```cmake
...
# because GLFW use cmake, we can do this
add_subdirectory(deps/glfw-3.3.2)
...
# include headers
target_include_directories(LearnOpenGL PRIVATE deps/glfw-3.3.2/include)
...
```

### GLAD
Location of most OpenGL functions in not known at compile-time and needs to be to be queried at run-time. In Windows it looks something like this

```c
// define function's prototype
typedef void (*GL_GENBUFFERS) (GLsizei, GLuint*);
// find the function and assign it to a function pointer
GL_GENBUFFERS glGenBuffers = (GL_GENBUFFERS)wglGetProcAddress("glGenBuffers");
// function can now be called as normal
unsigned int buffer;
glGenBuffers(1, &buffer);
```
GLAD serves for this purpose

### Setting Up
go to https://glad.dav1d.de and use this settings
![](Learn%20OpenGL/87FA64EC-2FCA-4F5A-A606-D68F155A23B7.png)
then copy:
	* file  `glad.c`  to `src/` 
	* both folders  `glad` and `KHR` to `include`

### Setting All Up
#### macOS
CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.17)
project(LearnOpenGL)

set(CMAKE_CXX_STANDARD 14)

add_executable(LearnOpenGL src/main.cpp src/glad.c)
find_package(OpenGL REQUIRED)

add_subdirectory(deps/glfw-3.3.2)

target_include_directories(LearnOpenGL PRIVATE include)
target_include_directories(LearnOpenGL PRIVATE deps/glfw-3.3.2/include)

target_link_libraries(LearnOpenGL glfw)
target_link_libraries(LearnOpenGL OpenGL==GL)
```

## Hello Window
==Be sure to include GLAD before GLFW==

<a href='Learn%20OpenGL/Hello%20Window.cpp'>Hello Window.cpp</a>

How it works:
1. First initialize GLFW using `glfwInit`
2. Configure GLFW using `glfwWindowHint`, first arg is option to configure, the second is value to set. List of possible option can be found in [GLFW's window handling](https://www.glfw.org/docs/latest/window.html#window_hints)
3. Create a window with size 800x600 and title "LearnOpenGL". If failed output to console, clean up resource and close the program
4. Tell GLFW to make the window's context the main context on current thread
5. Pass GLAD function to load the address of OpenGL function pointer to GLFW
6. Set the viewport from lower left corner f the window with size 800x600. OpenGL transform processed  2D coordinates to coordinate on screen, i.e. (-0.5,0.5)  is mapped to (200, 450) with (0, 0) is mapped to (400, 300)
7.  We register a callback function to  GLFW to resize viewport  each time window is resized
8. ==render loop== keep drawing image and handle user input, until told to stop
9. `glSwapBuffers` swap color buffer
10. Get input and set `glfwWindowShouldClose` to true if key pressed is escape
11. `glClear` is _state-using function_ clear color buffer with color specified in  _state-changing function_ `glClearColor`
	
![](Learn%20OpenGL/14205753-0FC8-4D5D-A545-FBF0E2754FA4.png)

## Hello Triangle
* everything in OpenGL is in 3D space
* Transforming 3D coordinates to 2D pixels managed by ==graphics pipeline==
* Processing cores on GPU run small program called ==shaders==

> Graphics pipeline stages  
![](Learn%20OpenGL/B07195BF-C423-45BB-8779-9D8A45572218.png)
* ==vertex== is a collection of data per 3D coordinate, represented using ==vertex attributes== chat can contain any data
* OpenGL require hint what kind of render types for the data, these hints called ==primitive==, i.e `GL_POINTS`,`GL_TRIANGLES`,`GL_LINE_STRIP`

**First** part of pipeline is ==vertex shader== that takes as input a single vertex, transform 3D coordinates into different 3D coordinates. 

**Second** the ==primitive assembly stage takes as input all the vertices from the vertex shader that form a primitive and assembles all the points in the primate shape given==; in this case a triangle

**Third** The output is passed to ==geometry shader that takes as input a collection of vertices that form a primitive and has the ability to generate other shapes== , in this case a second triangle out of the given shape

**Fourth** Then to ==rasterization tags where it maps the resulting primitive(s) to pixel on screen== resulting in fragments for fragment shader to use

> A fragment in OpenGL is all the data required for OpenGL to render a single pixel 

**Fifth** fragment shader calculate the final color of a pixel, ==usually this is the stage where all the advanced OpenGL effects occur and contain data about  the 3D scene==
**Sixth** After all color been determined, corresponding depth (and stencil) value of fragment is checked and uses those to check the resulting fragment is in front or behind other object. This stage also check alpha and blends the object

==For almost all cases we only have to work with vertex and fragment shader with geometry shader is optional and usually left by default==

### Vertex input
Defined

```c
float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.0f, 0.5f, 0.0f
};
```

We send them to vertex shader by creating memory  that managed by vertex buffer objects (VBO). Just like any object, this buffer has unique ID, generated using

```c
unsigned int VBO;
glGenBuffers(1, &VBO);
```

Vertex buffer object is `GL_ARRAY_BUFFER`, OpenGL allow binding to several buffers

```c
glBindBuffer(GL_ARRAY_BUFFER, VBO);
```

then we can copy the defined vertex data using

```c
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW)
```

the last parameter accept:
* GL_STREAM_DRAW set once, used at most a few times
* GL_STATIC_DRAW set once, used many times
* GL_DYNAMIC_DRAW data change and used a lot

### Vertex Shader

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
```

to compile above shader

```c
// to dynamically compile shader at runtime, we need to create 
//  a shader object
unsigned int vertexShader;
vertexShader = glCreateShader(GL_VERTEX_SHADER);

// next, attach shader source code then compile
glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
glCompileShader(vertexShader);
```

### Fragment Shader
fragment shader is all about calculating the color output of pixel

```glsl
#version 330 core
out vect FragColor;

void main()
{
	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
```

to compile above fragment shader is similar to vertex shader

```c
unsigned int fragmentShader;
fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
glshaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
glCompileShader(vertexShader);
```

### Shader Program
A shader program object is the final linked version of multiple shader. The compiled shader  need to be linked to be used then activate the shader program when rendering objects.
When linking shaders, it links the output of each shader to the inputs of next shader

```c
unsigned int shaderProgram;
shaderProgram = glCreateProgram();

glAttachShader(shaderProgram, vertexShader);
glAttachShader(shaderProgram, fragmentShader);
glLinkProgram(shaderProgram);

// We can delete linked shader
glDeleteShader(vertexShader);
glDeleteShader(fragmentShader);
```

to activate the program

```c
glUseProgram(shaderProgram);
```

### Linking Vertex Attributes
![](Learn%20OpenGL/vertex_attribute_pointer.png)
OpenGL need to be told how the vertex data need to be interpret before rendering

```c
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
```

* first parameter specifies which vertex to configure. the location is specified in `position` vertex attribute in vertex shader with `layout (location = 0)`
* second parameter is size fo vertex attribute i.e `vec3`
* third parameter is type of data. `vec` is GLSL is `GL_FLOAT`
* fourth argument specifies if the data want to be normalized
* fifth argument is strider, space between consecutive vertex attribute
* last parameter is offset where the position data begin in buffer

### Vertex Array Object
Vertex Array Object stores:
* calls to `glEnableVertexAttribArray` or `glDisableVertexAttribArray`
* Vertex attribute config via `glVertexAttribPointer`
* Vertex buffer objects associated with vertex attributes by calls to `glVertexAttribPointer`
![](Learn%20OpenGL/vertex_array_objects.png)
to generate and bind VAO is similar to VBO

```c
unsigned int VAO;
glGenVertexArrays(, &VAO);

glBindVertexArray(VAO);
```

to draw the object, OpenGL provide `glDrawsArrays` function that draws primitive function that draws primitives using currently active shader.

```c
glUseProgram(shaderProgram);
glBindVertexArray(VAO);
glDrawArrays(GL_TRIANGLES, 0, 3);
```

### Element Buffer Objects
Draw a rectangle	can be done using two triangles

```c
float vertices[] = {
	// first triangle
	0.5f, 0.5f, 0.0f, // top right
	0.5f, -0.5f, 0.0f, // bottom right
	-0.5f, 0.5f, 0.0f, // top left
	// second triangle
	0.5f, -0.5f, 0.0f, // bottom right
	-0.5f, -0.5f, 0.0f // bottom left
	-0.5f, 0.5f, 0.0f // top left
};
```

there 50% overhead in the code above because bottom right and top left is specified twice. to solve this

```c
float vertices[] = {
	// first triangle
	0.5f, 0.5f, 0.0f, // top right
	0.5f, -0.5f, 0.0f, // bottom right
	-0.5f, -0.5f, 0.0f // bottom left
	-0.5f, 0.5f, 0.0f // top left
};
unsigned int indices[] = {
	0, 1, 3, // first triangle
	1, 2, 3 // second triangle
};
```

next create EBO

```c
unisgned int EBO;
glGenBuffers(1, &EBO);

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
```

![](Learn%20OpenGL/vertex_array_objects_ebo.png)
`glDrawElements` takes its indices from EBO currently bound to `GL_ELEMENT_ARRAY_BUFFER`. Vertex array object keeps track of element of element buffer object binding. The las element buffer object bound while VAO is bound is stored as VAO's element buffer object. Binding to a VAO also automatically binds that EBO.

Full Code:
<a href='Learn%20OpenGL/Hello%20Rectangle.cpp'>Hello Rectangle.cpp</a>

![](Learn%20OpenGL/hellotriangle2.png)

#### Practice
/Learn OpenGL/Hello Triangle Exercise 1.cpp 'Hello Triangle Exercise 1.cpp'
/Learn OpenGL/Hello Triangle Exercise 2.cpp 'Hello Triangle Exercise 2.cpp'
/Learn OpenGL/Hello Triangle Exercise 3.cpp 'Hello Triangle Exercise 3.cpp'

## Shader
### GLSL 
typical shader

```glsl
#version version_number
in type in_variable_name;
in type in_variable_name;

out type out_variable_name;

uniform type uniform_name;

void main()
{
	// process input(s) and do some weird graphics stuff
	...
	// output processed stuff to output variable
	out_variable_name = weird_stuff_we_processed;
}
```

OpenGL guarantees at leas 16 4-component vertex attributes available, which can be retrieve by

```c
int nrAttributes;
glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
std==cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std==endl;
```

### Types
GLSL has most basic type like C: int, float, double, uint, bool, and two container types: vectors and matrices
#### Vectors
* `vecn`, n floats
* `bvecn`, n booleans
* `ivecn`, n integers
* `uvecn`, n unsigned integers
* `dvecn`, n doubles

vector allow swizzling

```glsl
vec2 someVec;
vec4 differentVec = someVec.xyxx;
vec3 anotherVec = differentVec.zyw;
vec4 otherVec = someVec.xxxx+ anotherVec.yxzy;

vec2 vec = vc2(0.5, 0.7);
vec4 result = vect(vect, 0.0, 0.0);
vec4 otherResult = vec4(result.xyz, 1.0);
```

#### Ins and outs
To send data from one to another, output has to be declared in the sending shader and a similar input in the receiving shader, when type and names are equal, OpenGL will link those variables together and then it is possible to send data between shader

vertex shader

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;

out vec4 vertexColor;

void main()
{
	gl_Position = vec4(aPos, 1,0);
	vertexColor = vec4(0.5, 0.0, 0.0, 1.0);
}
```

fragment shader

```glsl
#version 330 core
out vec4 FragColor;

in vec4 vertexColor;

void main()
{
	FragColor = vertexColor;
}
```

![](Learn%20OpenGL/shaders.png)

### Uniforms
Uniforms are another way to pass data from CPU to the shaders on GPU, but slightly different compared to vertex attributes. Uniforms are global. meaning unique per shader program and available to every shader in the program. Uniforms keep their value until reset or updated

```glsl
#version 330 core
out vec4 FragColor;

uniform vec4 ourColor;

void main()
{
	FragColor = ourColor;
}
```

```c
float timeValue = glfwGetTime();
float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
gluserProgram(shaderProgram);
glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
```

## Textures
In order to map a texture, each vertex need to have a ==texture coordinate==. It range from 0 to 1 in x and y axis. Retrieving texture color using texture coordinates is called ==sampling==.

![](Learn%20OpenGL/tex_coords.png)

```c
float textCoords[] = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.5f, 1.0f
};
```

### Texture Wrapping
By default if texture coordinate outside of (0, 0) to (1, 1), OpenGL will repeat the texture images
* GL_REPEAT
* GL_MIRRORED_REPEAT
* GL_CLAMP_TO_EDGE
* GL_CLAMPT_TO_BORDER

```c
// axis s, t, r => x, y, z
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
```

### Texture Filtering
```c
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
```

#### Mipmaps
Collection of texture where each subsequent texture is twice as small to the previous one

/Learn OpenGL/mipmaps.png 'mipmaps'

OpenGL provide `glGenerateMipmaps`

### Loading and creating textures
/Learn OpenGL/26F50D8F-CFD9-4B74-B847-A32DC303835D.png

<a href='Learn%20OpenGL/Texture.cpp'>Texture.cpp</a>

### Texture Units
OpenGL allows to use more than 1 texture in shaders, and should have at least 16 texture units.
```c
glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
glBindTexture(GL_TEXTURE_2D, texture);
```

### Exercise
- [ ] 1
- [ ] 2
- [ ] 3



#Learn/Learn OpenGL#