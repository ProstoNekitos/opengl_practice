#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include "Window.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * //TODO:
 * Make callback scene methods
 * Decide where to put skybox
 * Object inheritance
 * Model (mesh group)
 * Group (model group)
 * The fuck 2do with shadows
 * Default path for resources (do we need it?)
 * Fix terrain generators
 * Make water great again
 */

int main()
{
    //Widnow init & settings
    Window window(3,3);

    Window::initGlad();
    window.setMouseCallback(Window::mouse_callback);
    window.setFBCallback(Window::framebuffer_size_callback);
    window.setScrollCallback(Window::scroll_callback);

    //OpenGL global settings
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    //glEnable(GL_CULL_FACE); ///< TODO optimize indices generation

    glm::mat4 projection = glm::perspective(glm::radians(window.camera.Zoom), (float)window.width / (float)window.height, 0.1f, 100.0f);

    LightScene scene1;

    while ( !window.mustBeClosed() )
    {
        double currentFrame = glfwGetTime();
        window.deltaTime = currentFrame - window.lastFrame;
        window.lastFrame = currentFrame;

        window.processInput();

        scene1.render(window.camera, projection);

        glfwSwapBuffers(window.window);
        glfwPollEvents();
    }

    return 0;
}
