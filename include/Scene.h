#ifndef UNTITLED_SCENE_H
#define UNTITLED_SCENE_H

#include "Mesh.h"
#include "CubeMesh.h"
#include "Camera.h"
#include "Resources.h"

/**
 * 2lazy2implement right now
 */
class Scene
{
public:
    virtual void addObject(){};
    virtual void render(Camera cam, glm::mat4 projection){};

    /**
     * Default resource loading function. TODO: might be a good idea to put it in constructor
     */
    virtual void loadResources(){
        loadShaders();
        loadTextures();
        loadModels();
    }

    ///< Scene-specific functions (we don't need to store dick model in lego duplo scene (for example))
    virtual void loadShaders(){};
    virtual void loadTextures(){};
    virtual void loadModels(){};

    void terminate()
    {
        for(auto& a : meshes)
        {
            a.terminate();
        }
        Resources::clear();
        skybox.terminate();
    }

    virtual ~Scene()
    {
        terminate();
    }
    //virtual void sceneCallback(); //might be useful for specific (per scene) input

    std::vector<Mesh> meshes;

    //Camera camera; still don't sure do we need separate camera for every scene or we don't
    CubeMesh skybox;
};

#endif //UNTITLED_SCENE_H
