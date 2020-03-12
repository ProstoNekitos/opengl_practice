#ifndef UNTITLED_SCENE_H
#define UNTITLED_SCENE_H

#include "Mesh.h"
#include "CubeMesh.h"
#include "Camera.h"

/**
 * 2lazy2implement right now
 */
class Scene
{
public:
    virtual void addObject(){};
    virtual void render(Camera cam, glm::mat4 projection){};

    void terminate()
    {
        for(auto& a : meshes)
        {
            a.terminate();
        }

        skybox.terminate();
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
    virtual void sceneCallback();

    std::vector<Mesh> meshes;
    Camera camera;
    CubeMesh skybox;
};

#endif //UNTITLED_SCENE_H
