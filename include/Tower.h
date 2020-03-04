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
            vert.emplace_back(cos(sectorAngle),0,  sin(sectorAngle));
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

    std::map<float, float> heightRadMap = {
            {1.5,0.1},{1.55,0.3},{1.7,0.35},{1.85,0.3},{1.9,0.1}, //ball  ///<TODO sinusoid
            {1.43,0.4},{1.46,0.43},{1.49,0.4}, //upper ring
            {1.4, 0.35}, {.5, 0.4}, //slide ///<TODO sinusoid
            {-0.8, 1}, {-0.6, 1}, //above bottom ring before slide
            {-0.87, .92}, {-0.84,.9}, {-0.81,.92}, //bottom ring
            {-1, 1}, {-0.9, 1} //bottom
    };

};

#endif //UNTITLED_TOWER_H
