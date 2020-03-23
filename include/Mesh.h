#ifndef MESH_H
#define MESH_H

#include "Shader.h"
#include "Texture.h"

#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include <vector>

using namespace std;

struct Vertex {
    Vertex() = default;
    Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& tcord)
    : Position(pos), Normal(norm), TexCoords(tcord){}
    glm::vec3 Position{};
    glm::vec3 Normal{};
    glm::vec2 TexCoords{};
    glm::vec3 Tangent{};
    glm::vec3 Bitangent{};
};

class Mesh {
public:
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture2D*> textures;
    unsigned int VAO{};

    Mesh() = default;

    Mesh(const vector<Vertex>& vert, const vector<unsigned int>& ind)
    {
        vertices = vert;
        indices = ind;
        setup();
    }

    void addTexture(Texture2D tex)
    {
        textures.emplace_back(&tex);
    }

    /**
     * Removes old textures, sets new (might be useful when 1 mesh is used in different places)
     * @param tex
     */
    void setTextures(initializer_list<Texture2D *> tex)
    {
        textures = tex;
    }

    /**
     * Set textures using uniform
     * @param shader textures destination
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
            switch (textures[i]->type)
            {
                case Texture2D::TYPE::DIFFUSE:
                    number = std::to_string(diffuseNr++);
                    name = "texture_diffuse";
                    break;
                case Texture2D::TYPE::SPECULAR:
                    number = std::to_string(specularNr++);
                    name = "texture_specular";
                    break;
                case Texture2D::TYPE::NORMAL:
                    number = std::to_string(normalNr++);
                    name = "texture_normal";
                    break;
                case Texture2D::TYPE::HEIGHT:
                    number = std::to_string(heightNr++);
                    name = "texture_height";
                    break;
            }
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            textures[i]->bind();
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
    }

    void terminate(){
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

protected:
    unsigned int VBO{}, EBO{};

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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

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