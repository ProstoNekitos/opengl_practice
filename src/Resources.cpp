#include "Resources.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <utility>

#include <stb_image.h>

std::map<std::string, Texture2D>    Resources::textures;
std::map<std::string, Shader>       Resources::shaders;
std::map<std::string, Mesh>         Resources::meshes;


//Shaders
Shader Resources::loadShader(const std::string &name, const char *vShaderFile, const char *fShaderFile, const char *gShaderFile)
{
    shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return shaders[name];
}

Shader Resources::getShader(const std::string& name)
{
    return shaders[name];
}

Shader Resources::loadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile)
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

//Textures
Texture2D Resources::loadTexture(const std::string &name, const char *file, LOADTEXAS channels)
{
    textures[name] = loadTextureFromFile(file, channels);
    return textures[name];
}

Texture2D Resources::getTexture(const std::string& name)
{
    return textures[name];
}

Texture2D Resources::loadTextureFromFile(const char *file, LOADTEXAS textureChannels)
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

//Meshes
Mesh Resources::loadMesh(const std::string &name, const char *file) {
    meshes[name] = loadMeshFromFile(file);
    return meshes[name];
}

Mesh Resources::loadMesh(const std::string &name, Mesh mesh) {
    meshes[name] = std::move(mesh);
    return meshes[name];
}

Mesh Resources::getMesh(const std::string &name) {
    return meshes[name];
}

Mesh Resources::loadMeshFromFile(const char *file) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile( file,
                                              aiProcess_CalcTangentSpace       |
                                              aiProcess_Triangulate            |
                                              aiProcess_JoinIdenticalVertices  |
                                              aiProcess_SortByPType);
    if( !scene )
        throw std::runtime_error("Cannot load mesh");

    std::vector<Mesh> mehsVec;

    for(size_t i = 0; i < scene->mNumMeshes; ++i)
    {
        Mesh mesh;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        for( size_t v = 0; v < scene->mMeshes[i]->mNumVertices; ++v )
        {
            vertices.emplace_back(Vertex(
                    {scene->mMeshes[i]->mVertices[v].x, scene->mMeshes[i]->mVertices[v].y, scene->mMeshes[i]->mVertices[v].z},
                    {scene->mMeshes[i]->mNormals[v].x, scene->mMeshes[i]->mNormals[v].y, scene->mMeshes[i]->mNormals[v].z},
                    {scene->mMeshes[i]->mTextureCoords[0][v].x,scene->mMeshes[i]->mTextureCoords[0][v].y})
            );
        }

        for( size_t v = 0; v < scene->mMeshes[i]->mNumFaces; ++v )
        {
            for( size_t ind = 0; ind < scene->mMeshes[i]->mFaces[v].mNumIndices; ++ind )
                indices.push_back(scene->mMeshes[i]->mFaces[v].mIndices[ind]); //TODO refactor with insert
        }
        std::cout << '\n';
        mehsVec.emplace_back(vertices, indices);
    }

    return mehsVec[0];
}

//Util
void Resources::clear()
{
    for (auto& iter : shaders)
        glDeleteProgram(iter.second.ID);

    for (auto& iter : textures)
        glDeleteTextures(1, &iter.second.ID);
}