#ifndef UNTITLED_SCENE_H
#define UNTITLED_SCENE_H

#include <vector>
#include "Mesh.h"

#include "Skybox.h"

/**
 * 2lazy2implement right now
 */
class Scene
{
    Scene();

    virtual void addObject();
    virtual void render();

    std::vector<Mesh> meshes;
    Skybox skybox;
};

#endif //UNTITLED_SCENE_H