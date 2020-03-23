#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include "Window.h"


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
 * Make camera одной инстанцией на весь проект, я забыл как оно называется
 */

int main()
{
    //Widnow init & settings
    Window window(3,3);

    Window::initGlad();
    window.setMouseCallback(Window::mouse_callback);
    window.setFBCallback(Window::framebuffer_size_callback);
    window.setScrollCallback(Window::scroll_callback);

    glm::mat4 projection = glm::perspective(glm::radians(window.camera.Zoom), (float)window.width / (float)window.height, 0.1f, 100.0f);

    LightScene scene1;

    while ( !window.mustBeClosed() )
    {
        double currentFrame = glfwGetTime();
        window.deltaTime = currentFrame - window.lastFrame;
        window.lastFrame = currentFrame;

        window.processInput();

        scene1.update(window.camera, projection, window.width, window.height);

        glfwSwapBuffers(window.window);
        glfwPollEvents();
    }

    return 0;
}
