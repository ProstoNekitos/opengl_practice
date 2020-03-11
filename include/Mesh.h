#ifndef MESH_H
#define MESH_H

#include "Shader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include <vector>

using namespace std;

struct Vertex {
    Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& tcord) : Position(pos), Normal(norm), TexCoords(tcord){}
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

enum en_type:int {DIFFUSE, SPECULAR, NORMAL, HEIGHT};

struct Texture {
    unsigned int id;
    en_type type = en_type::DIFFUSE;

    static unsigned int loadTexture(char const * path)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format = GL_RGB;
            switch (nrComponents){
                case 1:
                    format = GL_RED;
                case 3:
                    format = GL_RGB;
                case 4:
                    format = GL_RGBA;
                default:
                    break;
            }

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }

};

class Mesh {
public:
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    unsigned int VAO;

    Mesh(const vector<Vertex>& vert, const vector<unsigned int>& ind, const vector<Texture>& tex)
    {
        vertices = vert;
        indices = ind;
        textures = tex;
        setup();
    }

    Mesh(const std::vector<float>& vert, const std::vector<float>& normals, const std::vector<float>& texCoords,
            const std::vector<unsigned int>& ind, const vector<Texture>& tex = vector<Texture>())
    {
        for(size_t i = 0; i < vert.size() - 2; i += 3)
            vertices.emplace_back(glm::vec3(vert[i], vert[i+1], vert[i+2]), glm::vec3(normals[i], normals[i+1], normals[i+2]), glm::vec2() );

        std::vector<glm::vec2> buffTexCord;

        for(size_t i = 0; i < texCoords.size() - 1; i += 2)
            buffTexCord.emplace_back(texCoords[i], texCoords[i + 1]);

        for(size_t i = 0; i < buffTexCord.size(); ++i)
            vertices[i].TexCoords = buffTexCord[i];

        indices = ind;
        textures = tex;

        setup();
    }

    void setupTextures(const vector<Texture>& tex){
        textures = tex;
    }

    /**
     * Set textures using uniform
     * @param shader Textures destination
     */
    void render(Shader shader)
    {
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int heightNr   = 1;

        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            string number;
            std::string name;
            switch (textures[i].type){
                case DIFFUSE:
                    number = std::to_string(diffuseNr++);
                    name = "texture_diffuse";
                    break;
                case SPECULAR:
                    number = std::to_string(specularNr++);
                    name = "texture_specular";
                    break;
                case NORMAL:
                    number = std::to_string(normalNr++);
                    name = "texture_normal";
                    break;
                case HEIGHT:
                    number = std::to_string(heightNr++);
                    name = "texture_height";
                    break;
            }

            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
    }

    /**
     * Delete buffers
     */
    void terminate(){
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

private:
    unsigned int VBO, EBO;

    /**
     * Setup VAO, VBO & EBO
     */
    void setup()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

        glBindVertexArray(0);
    }
};


#endif