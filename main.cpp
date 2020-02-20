#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main() {
    if( !glfwInit() ){
        std::cout << "Some bad shit happened. glfw was not initialized \n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    GLFWwindow* window = glfwCreateWindow(100,100, "Blank", nullptr, nullptr);

    if( !window ){
        std::cout << "Some bad shit happened. Window was not created \n";
        return 1;
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window))
    {
        // Keep running
    }


    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
