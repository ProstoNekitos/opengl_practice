#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glad/glad.h>

#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"

class Resources
{
public:
    enum LOADTEXAS: int {asItIs = 0, grey = 1, alphaGrey = 2, rgb = 3, rgba = 4};

    static std::map<std::string, Shader>    shaders;
    static std::map<std::string, Texture2D> textures;
    static std::map<std::string, Mesh> meshes;


    static Shader loadShader(const std::string &name, const char *vShaderFile, const char *fShaderFile, const char *gShaderFile = nullptr);
    static Shader getShader(const std::string& name);

    static Texture2D loadTexture(const std::string &name, const char *file, LOADTEXAS channels = rgb);
    static Texture2D getTexture(const std::string& name);

    static Mesh loadMesh(const std::string &name, const char *file);
    static Mesh loadMesh(const std::string &name, Mesh mesh);
    static Mesh getMesh(const std::string& name);

    static void clear();

private:
    Resources() = default;

    static Shader    loadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile = nullptr);
    static Texture2D loadTextureFromFile(const char *file, LOADTEXAS textureChannels);
    static Mesh loadMeshFromFile(const char *file);

};

#endif

