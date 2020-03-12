#ifndef UNTITLED_WINDOW_H
#define UNTITLED_WINDOW_H

#include "Scene.h"
#include "Camera.h"

#include <GLFW/glfw3.h>

#include <stdexcept>

class Window
{

public:
    Window(int major, int minor, Camera* cam = new Camera(), size_t w = 800, size_t h = 600)
    : width(w), height(h), camera(cam)
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
        #endif

        window = glfwCreateWindow(width, height, "Nick", nullptr, nullptr);

        if ( !window )
        {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(window);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    void setMouseCallback(GLFWcursorposfun callback)
    {
        glfwSetCursorPosCallback(window, callback);
    }

    void setScrollCallback(GLFWscrollfun callback)
    {
        glfwSetScrollCallback(window, callback);
    }

    void setFBCallback(GLFWframebuffersizefun callback)
    {
        glfwSetFramebufferSizeCallback(window, callback);
    }

    static void framebuffer_size_callback(GLFWwindow*, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    /**
     * Deletes old one and set pointer to new
     * @param cam pointer to the new cam
     * \todo troubles if a pointer to stack allocated object was passed, might be a good idea to create array of cams to swap between them
     */
    void setCamera(Camera* cam)
    {
        delete camera;
        camera = cam;
    }

    /**
     * Initializes GLAD; Btw it's kinda bad decision to leave it there
     */
    static void initGlad(){
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            throw std::runtime_error("Failed to initialize GLAD");
    }

    size_t height;
    size_t width;

    Camera* camera;

    std::vector<Scene> scenes;

    GLFWwindow* window = nullptr;
};

#endif //UNTITLED_WINDOW_H
