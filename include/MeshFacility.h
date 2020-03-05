#ifndef UNTITLED_MESHFACILITY_H
#define UNTITLED_MESHFACILITY_H

#include <cmath>
#include <algorithm>
#include <map>

#include <Mesh.h>

class MeshFacility
{
public:
    static std::vector<glm::vec3> generateSurface(size_t sectorCount)
    {
        float sectorStep = 2.0f * M_PI / sectorCount;
        float sectorAngle;  // radian

        std::vector<glm::vec3> vert;
        for(int i = 0; i < sectorCount; ++i)
        {
            sectorAngle = i * sectorStep;
            vert.emplace_back(cos(sectorAngle),0,  sin(sectorAngle));
        }
        return vert;
    }

    static Mesh generateCylinder(unsigned int sectorCount, const std::map<float, float>& heightRadMap, const std::string& texturePath)
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
                vert += glm::vec3(0,hr.first,0);
                vertices.emplace_back(Vertex{vert, norm, tex});
            });
        }

        std::vector<unsigned int> indices;

        for(unsigned int i = 0; i < vertices.size() - sectorCount; ++i )
        {
            unsigned int lvl = (i / sectorCount + 1);
            unsigned int lvlDelim = lvl * sectorCount;
            unsigned int diagonalToI = i + sectorCount + 1;
            unsigned int lastInd = i + 1;

            if( lastInd == lvlDelim )
                lastInd -= sectorCount;

            if( diagonalToI == lvlDelim + sectorCount ) ///< Case between lvls
                diagonalToI -= sectorCount; ///<FUCK THIS AS WELL

            unsigned int buffIndicies[6] = {
                    i, diagonalToI, i+sectorCount, ///< fuck this btw
                    i, diagonalToI, lastInd
            };

            indices.insert(indices.end(), buffIndicies, buffIndicies + 6);
        }

        return std::move( Mesh(vertices, indices, std::vector<Texture>( loadTexture( texturePath.c_str() ) )) );
    }
};

#endif //UNTITLED_MESHFACILITY_H
