#ifndef UNTITLED_TERRAIN_H
#define UNTITLED_TERRAIN_H

#include "Mesh.h"
#include "FastNoise.h"

/**
 * Might a bad idea to inherit from Mesh
 */
class Terrain : public Mesh
{
public:
    Terrain(unsigned int w = 5, unsigned int h = 5, unsigned int ofset = 15)
    : width(w), height(h), offset(ofset)
    {
        generateIndices();
        heightMap = generateHeightMap();
        generateVertices();
        setup();
    }

    void HeightMapFromFile(const char* file);

    float** generateHeightMap(int seed = 1337)
    {
        FastNoise noise(seed);
        noise.SetNoiseType(FastNoise::Perlin);
        float** nm = new float*[width];
        for(size_t x = 0; x < width; ++x)
        {
            nm[x] = new float[height];
            for(size_t y = 0; y < height; ++y)
            {
                double nx = x/width - 0.5;
                double ny = y/height - 0.5;
                nm[x][y] = noise.GetNoise(x, y) /*+
                        0.5 * noise.GetNoise(2 * nx, 2 * ny) +
                        0.25 * noise.GetNoise( 2* nx, 2 * ny)*/;
                std::cout << nm[x][y] << ' ';
            }
        }
        return nm;
    }

    /** The most simple way
     *  0---1---2
     *  |\  |\  |
     *  | \ | \ |
     *  |  \|  \|
     *  3---4---5
     *  Indices generated square after square
     */
    void generateIndices()
    {
        unsigned int vertexCount = width * height;
        for (unsigned int i = 0; i < vertexCount - width; i++)  ///< except for the last row
        {
            if( (i + 1) % width == 0 && i )
                continue;

            unsigned int topLeft = i;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = i + width;
            unsigned int bottomRight = bottomLeft + 1;

            indices.insert(indices.end(), {topLeft, bottomLeft, bottomRight}); ///< First triangle
            indices.insert(indices.end(), {topLeft, topRight, bottomRight}); ///< Second triangle
        }
        std::cout << "Place for your breakpoint\n";
    }

    void generateVertices()
    {
        for(size_t i = 0; i < width; ++i){
            for(size_t j = 0; j < height; ++j)
            {
                vertices.emplace_back(
                        glm::vec3((width/2 - i)/width, heightMap[i][j], (height/2 - j)/height),
                        glm::vec3(0,0,0 ),
                        glm::vec2( i/width, j/height ));
            }
        }
        std::cout << '\n' << vertices.size() << '\n';
    }

    void setWidth()
    {

    }

    void setHeight()
    {

    }

    void setOffset()
    {}

    ~Terrain(){
        for(size_t i = 0; i < width; ++i)
        {
            delete[] heightMap[i];
        }
        delete[] heightMap;
    }

private:

    unsigned int width; ///< In vert number
    unsigned int height; ///< In vert number
    unsigned int scale = 1;

    float** heightMap = nullptr; ///< Do we really need to store it?
    float offset = 15; ///<between verts

    float seaLvl; ///< Idk where to use it except for coloring
};

#endif
