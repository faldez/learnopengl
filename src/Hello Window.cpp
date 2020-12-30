#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

int main()
{
    // First initialize GLFW 
    glfwInit();
    // Configure GLFW
    // first arg is option to configure, the second is value to set. 
    // List of possible option can be found in https://www.glfw.org/docs/latest/window.html#window_hints)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create a window with size 800x600 and title "LearnOpenGL"
    // If failed output to console, clean up resource and close the program
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Tell GLFW to make the window's context the main context on current thread
    glfwMakeContextCurrent(window);

    // Pass GLAD function to load the address of OpenGL function pointer to GLFW
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Set the viewport from lower left corner f the window with size 800x600. 
    // OpenGL transform processed  2D coordinates to coordinate on screen, 
    // i.e. (-0.5,0.5)  is mapped to (200, 450) with (0, 0) is mapped to (400, 300)
    glViewport(0, 0, 800, 600);

    // We register a callback function to  GLFW to resize viewport  each time window is resized
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // render loop 
    // Keep drawing image and handle user input, until told to stop
    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        // state-changing function to change state to specified color
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // state-using function to use state change above to clear color buffer 
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}