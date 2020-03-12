#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include "Window.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "lightScene.h"

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// timing
double deltaTime = 0.0f;
double lastFrame = 0.0f;

/**
 * //TODO:
 * Make callback scene methods
 * Merge texture & texture 2D
 * Decide where to put skybox
 * Object inheritance
 * Model (mesh group)
 * Group (model group)
 * The fuck 2do with shadows
 * Model load lib
 */

int main()
{
    //Widnow init & settings
    Window window(3,3);
    window.setCamera(&camera);
    Window::initGlad();
    window.setMouseCallback(mouse_callback);
    window.setFBCallback(Window::framebuffer_size_callback);
    window.setScrollCallback(scroll_callback);

    //OpenGL global settings
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    //glEnable(GL_CULL_FACE); ///< TODO optimize indices generation

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)window.width / (float)window.height, 0.1f, 100.0f);

    LightScene scene1;

    while (!glfwWindowShouldClose(window.window)) //Dumb
    {

        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window.window);

        scene1.render(camera, projection);
        window.setMouseCallback(scene1.mouse_callback);

        glfwSwapBuffers(window.window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow*, double xpos, double ypos)
{
    static bool firstMouse = true;
    static double lastX = 0;
    static double lastY = 0;
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow*, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
