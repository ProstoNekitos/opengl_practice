#ifndef UNTITLED_TOWER_H
#define UNTITLED_TOWER_H

#include <cmath>
#include <algorithm>
#include <map>

#include <Mesh.h>

class Tower
{
public:
    std::vector<glm::vec3> generateSurface(size_t sectorCount)
    {
        float sectorStep = 2.0f * M_PI / sectorCount;
        float sectorAngle;  // radian

        std::vector<glm::vec3> vert;
        for(int i = 0; i < sectorCount; ++i)
        {
            sectorAngle = i * sectorStep;
            vert.emplace_back(cos(sectorAngle), sin(sectorAngle), 0);
        }
        return vert;
    }

    Mesh buildVerticesSmooth(unsigned int sectorCount, const std::string& texturePath)
    {
        std::vector<glm::vec3> unitVertices = generateSurface(sectorCount);
        glm::vec3 translationMat(1);

        std::vector<Vertex> vertices;
        for( auto& hr : heightRadMap )
        {
            std::vector<glm::vec3> lvlVert = unitVertices;
            std::for_each(lvlVert.begin(), lvlVert.end(),[&hr, &vertices](glm::vec3 vert){
                glm::vec3 norm = vert;
                glm::vec2 tex = {0,0};
                vert *= hr.second; ///< At first scale down by rad, then inc height
                vert += glm::vec3(0,0,hr.first);
                vertices.emplace_back(Vertex{vert, norm, tex});
            });
        }

        std::vector<unsigned int> indices;

        for(unsigned int i = 0; i < vertices.size() - sectorCount; ++i )
        {
            unsigned int lvlDelim = (i / sectorCount + 1) * sectorCount;
            unsigned int buffIndicies[6] = {
                    i, i+sectorCount, (i+1) % lvlDelim + sectorCount, ///< fuck this btw
                    i, (i+1) % lvlDelim + sectorCount, (i+1) % lvlDelim
            };
            indices.insert(indices.end(), buffIndicies, buffIndicies + 6);
        }

        return std::move( Mesh(vertices, indices, std::vector<Texture>( loadTexture( texturePath.c_str() ) )) );
    }

    std::map<float, float> heightRadMap = {{1,1},{.5,1}, {.45,.1}};

};

#endif //UNTITLED_TOWER_H
