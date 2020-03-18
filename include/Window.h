#ifndef UNTITLED_WINDOW_H
#define UNTITLED_WINDOW_H

#include "lightScene.h"
#include "Camera.h"

#include <GLFW/glfw3.h>
#include <stdexcept>

class Window
{

public:
    Window(int major, int minor, size_t w = 800, size_t h = 600)
    : width(w), height(h)
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

        glfwSetWindowUserPointer(window, &camera);

    }

    ~Window(){
        glfwTerminate();
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

    void processInput()
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(Camera::FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(Camera::BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(Camera::LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(Camera::RIGHT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            camera.ProcessKeyboard(Camera::UP, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
            camera.ProcessKeyboard(Camera::DOWN, deltaTime);
    }

    static void mouse_callback(GLFWwindow* win, double xpos, double ypos)
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

        static_cast<Camera*>(glfwGetWindowUserPointer(win))->ProcessMouseMovement(xoffset, yoffset);
    }

    static void scroll_callback(GLFWwindow* win, double, double yoffset)
    {
        static_cast<Camera*>(glfwGetWindowUserPointer(win))->ProcessMouseScroll(yoffset);
    }

    bool mustBeClosed(){
        return glfwWindowShouldClose(window);
    }

    static void initGlad(){
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            throw std::runtime_error("Failed to initialize GLAD");
    }

    size_t height;
    size_t width;

    double deltaTime = 0.0f;
    double lastFrame = 0.0f;

    Camera camera;

    std::vector<Scene> scenes;

    GLFWwindow* window = nullptr;
};

#endif //UNTITLED_WINDOW_H
