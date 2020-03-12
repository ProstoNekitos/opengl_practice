#include "ResourceManager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <stb_image.h>

// Instantiate static variables
std::map<std::string, Texture2D>    ResourceManager::Textures;
std::map<std::string, Shader>       ResourceManager::Shaders;


Shader ResourceManager::LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name)
{
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
}

Shader ResourceManager::GetShader(const std::string& name)
{
    return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const char *file, const std::string& name, LOADTEXAS channels)
{
    Textures[name] = loadTextureFromFile(file, channels);
    return Textures[name];
}

Texture2D ResourceManager::GetTexture(const std::string& name)
{
    return Textures[name];
}

void ResourceManager::Clear()
{
    for (auto& iter : Shaders)
        glDeleteProgram(iter.second.ID);

    for (auto& iter : Textures)
        glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    try
    {
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(fShaderFile);
        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();

        vertexShaderFile.close();
        fragmentShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        if (gShaderFile)
        {
            std::ifstream geometryShaderFile(gShaderFile);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }

    catch (std::exception& e)
    {
        std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
    }

    const GLchar *vShaderCode = vertexCode.c_str();
    const GLchar *fShaderCode = fragmentCode.c_str();
    const GLchar *gShaderCode = geometryCode.c_str();

    Shader shader;
    shader.compile(vShaderCode, fShaderCode, gShaderFile ? gShaderCode : nullptr);
    return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char *file, LOADTEXAS textureChannels)
{
    Texture2D texture;

    int width, height;
    int imageChannels;

    unsigned char* data = stbi_load(file, &width, &height, &imageChannels, textureChannels);

    if( !data )
        std::cout << "Can't load texture from " << file << '\n';

    switch (imageChannels)
    {
        case 1:
            texture.internalFormat = GL_RED;
            break;
        case 2:
            texture.internalFormat = GL_RG;
            break;
        case 3:
            texture.internalFormat = GL_RGB;
            break;
        case 4:
            texture.internalFormat = GL_RGBA;
            break;
        default:
            texture.internalFormat = GL_RGB;
    }

    switch (textureChannels)
    {
        case asItIs:
            texture.textureFormat = texture.internalFormat;
            break;
        case grey:
            texture.textureFormat = GL_RED;
            break;
        case alphaGrey:
            texture.textureFormat = GL_RG;
            break;
        case rgb:
            texture.textureFormat = GL_RGB;
            break;
        case rgba:
            texture.textureFormat = GL_RGBA;
            break;
    }

    texture.generate(width, height, data);
    stbi_image_free(data);

    return texture;
}