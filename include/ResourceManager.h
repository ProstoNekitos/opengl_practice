#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>

#include <glad/glad.h>

#include "Texture.h"
#include "Shader.h"

class ResourceManager
{
public:
    enum LOADTEXAS: int {asItIs = 0, grey = 1, alphaGrey = 2, rgb = 3, rgba = 4};

    static std::map<std::string, Shader>    Shaders;
    static std::map<std::string, Texture2D> Textures;

    static Shader   LoadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, std::string name);
    static Shader   GetShader(const std::string& name);

    static Texture2D LoadTexture(const char *file, const std::string& name, LOADTEXAS channels = LOADTEXAS::asItIs);
    static Texture2D GetTexture(const std::string& name);

    static void      Clear();

private:
    ResourceManager() = default;
    static Shader    loadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile = nullptr);
    static Texture2D loadTextureFromFile(const char *file, LOADTEXAS textureChannels);
};

#endif

