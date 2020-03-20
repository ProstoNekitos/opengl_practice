#ifndef UNTITLED_TERRAIN_H
#define UNTITLED_TERRAIN_H

#include "Mesh.h"
#include "FastNoise.h"

//Needs only for random color selection
#include <random>
#include <iterator>

template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}


/**
 * TODO
 * Refactor vertices generator (we can generate 1/4 of them, then just flip 2 times)
 */

struct ColorVertex{
    ColorVertex(Vertex v, glm::vec3 col)
    : vert(v), color(col){}
    Vertex vert;
    glm::vec3 color;
};

struct ZoneColor
{
    float bottomLine; ///< Zone bottom line
    glm::vec3 baseColor; ///< Base zone color
    std::vector<glm::vec3> horizontalMixColors; ///< Horizontal color shift
    glm::vec3 mixHorizontalColor()
    {
        std::random_device rd; //TODO make generator static
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        float alpha = dis(gen);
        return baseColor * alpha + (1 - alpha) * (*select_randomly(horizontalMixColors.begin(), horizontalMixColors.end()));
    }
};

/**
 * Might a bad idea to inherit from Mesh
 */
class Terrain
{
public:
    explicit Terrain(unsigned int w = 30, unsigned int h = 30, unsigned int ts = 15)
    : width(w), height(h), tileSize(ts),
    colors{
            {//snow
                0.8,
                {255,255,255}, //white
                {{126, 249, 255},
                 {115,194,251},
                 {176, 223,229},
                 {149, 200, 216}}
            },
            {//mountain
                    0.3,
                    {255,255,255},
                    {{181,170,157},
                            {65,71,74},
                            {0,0,0},
                            {129,132,125}}
            },
            {//greenZone
                    -0.3,
                    {255,255,255},
                    {{126, 249, 255}}
            },
            {//underWater
                    -1,
                    {255,255,255},
                    {{126, 249, 255}}
            }

    }
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

    void HeightMapFromFile(const char* file)
    {

    };

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
    [[nodiscard]] float** generateHeightMap(int seed = 3242123)
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

        addBumps(1, 0.2, nm); //Waves
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

        //Normalize (if >1 then 1, if <(-1) then -1)
        for(size_t x = 0; x < nmW; ++x)
        {
            for(size_t y = 0; y < nmH; ++y)
            {
                if( nm[x][y] < -1 ) nm[x][y] = -1;
                if( nm[x][y] > 1 ) nm[x][y] = 1;
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
                    Vertex(glm::vec3((fwidth/2.f - i)/fwidth, heightMap[i*tileSize][j*tileSize], (fheight/2.f - j)/fheight),
                           glm::vec3(0,0,0 ),
                           glm::vec2( i/fwidth, j/fheight )),
                    determineColor(heightMap[i*tileSize][j*tileSize])
                );
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

    /**
     * Set textures using uniform
     * @param shader textures destination
     */
    void render()
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
    }

    void terminate(){
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

public:

    //Rendering
    vector<ColorVertex> vertices;
    vector<unsigned int> indices;

private:

    void setup()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ColorVertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        //Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), (void*)offsetof(ColorVertex, vert));

        //Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), (void*)offsetof(ColorVertex, vert) );

        //Texture coordinates
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), (void*)(offsetof(ColorVertex, vert) + offsetof(Vertex, Normal)));

        //Tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), (void*)(offsetof(ColorVertex, vert) + offsetof(Vertex, Tangent)));

        //Bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), (void*)(offsetof(ColorVertex, vert) + offsetof(Vertex, Bitangent)));

        //Colors
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), (void*)offsetof(ColorVertex, color));

        glBindVertexArray(0);
    }

    glm::vec3 determineColor(const float& h)
    {
        for(auto& lvl : lvlColorMap)
            if( h > lvl.first )
            {
                auto col = *select_randomly(lvl.second.begin(), lvl.second.end());
                return {col.x / 255, col.y / 255, col.z / 255};
            }
        return {6,6,6};
    }

    std::vector< std::pair<float, std::vector<glm::vec3> > > lvlColorMap;

    std::vector<ZoneColor> colors;

    unsigned int width; ///< In vert number
    unsigned int height; ///< In vert number

    unsigned int VBO{}, EBO{}, VAO{};

    float** heightMap = nullptr; ///< Do we really need to store it?
    unsigned tileSize = 15; ///<between verts
};

#endif
