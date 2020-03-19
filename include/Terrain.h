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
    Terrain(unsigned int w = 30, unsigned int h = 30, unsigned int ts = 15)
    : width(w), height(h), tileSize(ts)
    {
        heightMap = generateHeightMap();
        generateVertices();
        generateIndices();
        setup();
    }

    void HeightMapFromFile(const char* file);

    float** generateHeightMap(int seed = 2424123)
    {
        FastNoise noise(seed);
        noise.SetNoiseType(FastNoise::Perlin);
        noise.SetFrequency(2);

        float fw = static_cast<float>(width)*tileSize;
        float fh = static_cast<float>(height)*tileSize;

        size_t nmW = (width + 1) * tileSize;
        size_t nmH = (height + 1) * tileSize;

        float** nm = new float*[nmW];
        for(size_t x = 0; x < nmW; ++x)
        {
            nm[x] = new float[nmH];
            for(size_t y = 0; y < nmH; ++y)
            {
                double nx = x/fw - 0.5;
                double ny = y/fh - 0.5;
                nm[x][y] = noise.GetNoise(nx, ny) +
                        0.5 * noise.GetNoise(2 * nx, 2 * ny) +
                        0.25 * noise.GetNoise( 2* nx, 2 * ny);
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

        for(unsigned int row = 0; row < height - 1; ++row)
        {
            for(unsigned int col = 0; col < width - 1; ++col)
            {
                unsigned topLeft = row * width + col;
                unsigned topRight = topLeft + 1;
                unsigned bottomLeft = (row + 1) * width + col;
                unsigned bottomRight = bottomLeft + 1;
                indices.insert(indices.end(),
                        {
                                topLeft,
                                bottomLeft,
                                bottomRight
                        });

                indices.insert(indices.end(),
                        {
                                topLeft,
                                topRight,
                                bottomRight
                        });
            }
        }
        std::cout << "Place for your breakpoint\n";
    }

    /** The most simple way
   *  0---1---2
   *  |   |   |
   *  |   |   |
   *  |   |   |
   *  3---4---5
   */
    void generateVertices()
    {
        auto fwidth  = static_cast<float>(width);
        auto fheight  = static_cast<float>(height);

        size_t num = 0;

        for(int j = 0; j < height; ++j)
        {
            for(int i = 0; i < width; ++i)
            {
                vertices.emplace_back(
                        glm::vec3((fwidth/2.f - i)/fwidth, heightMap[i*tileSize][j*tileSize], (fheight/2.f - j)/fheight),
                        glm::vec3(0,0,0 ),
                        glm::vec2( i/fwidth, j/fheight ));

                std::cout << num++ << ' ' << (fwidth/2.f - i)/fwidth << ' ' << (fheight/2.f - j)/fheight << '\n';
            }
            std::cout << '\n';
        }
    }

    void setWidth()
    {

    }

    void setHeight()
    {

    }

    void setTileSize()
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
    int tileSize = 15; ///<between verts

    float seaLvl; ///< Idk where to use it except for coloring
};

#endif
