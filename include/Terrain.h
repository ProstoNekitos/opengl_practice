#ifndef UNTITLED_TERRAIN_H
#define UNTITLED_TERRAIN_H

#include "Mesh.h"
#include "FastNoise.h"

/**
 * TODO
 * Maybe it is better to set color right here, not in shader
 * Refactor vertices generator (we can generate 1/4 of them, then just flip 2 times)
 */

/**
 * Might a bad idea to inherit from Mesh
 */
class Terrain
{
public:
    Terrain(unsigned int w = 30, unsigned int h = 30, unsigned int ts = 15)
    : width(w), height(h), tileSize(ts)
    {
        generateEverything();
        setup();
    }

    ~Terrain(){
        clearHeightMap();
    }

    void generateEverything()
    {
        heightMap = generateHeightMap();
        generateVertices();
        generateIndices();
    }

    void HeightMapFromFile(const char* file){};

    /**
     * Adds bumps to height map
     * @param frequency in Hz probably
     * @param impact how strong (tall) bumps would be
     */
    void addBumps(float frequency, float impact, float** hm)
    {
        FastNoise noise;
        noise.SetNoiseType(FastNoise::Simplex);
        noise.SetFrequency(frequency);

        float fw = static_cast<float>(width)*static_cast<float>(tileSize);
        float fh = static_cast<float>(height)*static_cast<float>(tileSize);
        size_t nmW = (width + 1) * tileSize;
        size_t nmH = (height + 1) * tileSize;

        for(size_t x = 0; x < nmW; ++x)
        {
            for(size_t y = 0; y < nmH; ++y)
            {
                double nx = x/fw - 0.5;
                double ny = y/fh - 0.5;

                hm[x][y] += impact * noise.GetNoise(nx, ny);
            }
        }
    }

    /**
     * Clears lod heightMap and generates new;
     * TODO: change this lib; 3 n^2 loop are terrifying
     * @param seed
     * @return
     */
    [[nodiscard]] float** generateHeightMap(int seed = 1337)
    {
        clearHeightMap();

        FastNoise noise;
        noise.SetNoiseType(FastNoise::Simplex);
        noise.SetFrequency(.5);
        noise.SetSeed(seed);

        float fw = static_cast<float>(width)*static_cast<float>(tileSize);
        float fh = static_cast<float>(height)*static_cast<float>(tileSize);

        size_t nmW = (width + 1) * tileSize;
        size_t nmH = (height + 1) * tileSize;


        auto** nm = new float*[nmW];
        for(size_t x = 0; x < nmW; ++x)
        {
            nm[x] = new float[nmH];
            for(size_t y = 0; y < nmH; ++y)
            {
                double nx = x/fw - 0.5;
                double ny = y/fh - 0.5;
                nm[x][y] = noise.GetNoise(nx, ny);
            }
        }

        addBumps(2, 0.5, nm); //Waves
        addBumps(4, 0.15, nm); //Medium bumps
        addBumps(16, 0.05, nm); //Little bumps

        /*There lies an exponent function which somehow chews my terrain
         * for(size_t x = 0; x < nmW; ++x)
        {
            for(size_t y = 0; y < nmH; ++y)
            {
                double nx = x/fw - 0.5;
                double ny = y/fh - 0.5;

                nm[x][y] += ;//std::pow(nm[x][y], 1.0000001);
                std::cout << nm[x][y] << ' ';
            }
        }*/

        //Get max & min height, normalize (if >1 then 1, if <(-1) then -1)
        for(size_t x = 0; x < nmW; ++x)
        {
            for(size_t y = 0; y < nmH; ++y)
            {
                if( nm[x][y] < -1 ) nm[x][y] = -1;
                if( nm[x][y] > 1 ) nm[x][y] = 1;

                if( nm[x][y] > maxHeight ) maxHeight = nm[x][y];
                if( nm[x][y] < minHeight ) minHeight = nm[x][y];
            }
        }

        return nm;
    }

    /**
     * Clears current indices and creates new
     * The most simple way
     *  0---1---2
     *  |\  |\  |
     *  | \ | \ |
     *  |  \|  \|
     *  3---4---5
     *  Indices generated square after square
     */
    void generateIndices()
    {
        indices.clear();
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

    /**
     * Clears previous vertices and creates new
     * The most simple way
   *  0---1---2
   *  |       |
   *  |       |
   *  |       |
   *  3---4---5
   */
    void generateVertices()
    {
        vertices.clear();
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
            }
        }
    }


    void clearHeightMap(){
        if( heightMap )
        {
            for(size_t i = 0; i < width; ++i)
                delete[] heightMap[i];
            delete[] heightMap;
        }
    }


    //Setters
    void setWidth(unsigned w)
    {
        width = w;
        generateEverything();
    }

    void setHeight(unsigned h)
    {
        height = h;
        generateEverything();
    }

    void setTileSize(unsigned ts)
    {
        tileSize = ts;
    }


    /**
 * Set textures using uniform
 * @param shader textures destination
 */
    void render(Shader shader)
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
    }
public:
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    unsigned int VAO{};

public:
    //Coloring
    float maxHeight = -1;
    float minHeight = 1;

private:

    unsigned int VBO{}, EBO{};

    unsigned int width; ///< In vert number
    unsigned int height; ///< In vert number
    unsigned int scale = 1;

    float** heightMap = nullptr; ///< Do we really need to store it?
    unsigned tileSize = 15; ///<between verts

    std::map<float, glm::vec3> lvlColor;
};

#endif
