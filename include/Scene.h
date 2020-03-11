#ifndef UNTITLED_SCENE_H
#define UNTITLED_SCENE_H

#include <vector>

#include "Mesh.h"
#include "CubeMesh.h"

#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

    std::vector<Mesh> meshes;
    CubeMesh skybox;
};

#endif //UNTITLED_SCENE_H
