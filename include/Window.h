#ifndef UNTITLED_WINDOW_H
#define UNTITLED_WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Camera.h>

class Window
{
    void init(int major, int minor);

    void setMouseCallback();
    void setScrollCallback();
    void setFBCallback();
};

#endif //UNTITLED_WINDOW_H
