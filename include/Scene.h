#ifndef UNTITLED_SCENE_H
#define UNTITLED_SCENE_H

#include <vector>
#include "Mesh.h"

#include "CubeMesh.h"

/**
 * 2lazy2implement right now
 */
class Scene
{
    Scene();

    virtual void addObject();
    virtual void render();

    std::vector<Mesh> meshes;
    CubeMesh skybox;
};

#endif //UNTITLED_SCENE_H
