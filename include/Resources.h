#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>

#include <glad/glad.h>

#include "Texture.h"
#include "Shader.h"

class Resources
{
public:
    enum LOADTEXAS: int {asItIs = 0, grey = 1, alphaGrey = 2, rgb = 3, rgba = 4};

    static std::map<std::string, Shader>    Shaders;
    static std::map<std::string, Texture2D> Textures;

    static Shader loadShader(const std::string &name,
            const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile = nullptr);
    static Shader   getShader(const std::string& name);

    static Texture2D loadTexture(const char *file, const std::string& name, LOADTEXAS channels = LOADTEXAS::asItIs);
    static Texture2D getTexture(const std::string& name);

    static void      clear();

private:
    Resources() = default;
    static Shader    loadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile = nullptr);
    static Texture2D loadTextureFromFile(const char *file, LOADTEXAS textureChannels);
};

#endif

