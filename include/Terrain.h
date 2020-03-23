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
    float betweenLvlAlpha;


    glm::vec3 mixHorizontalColor()
    {
        std::random_device rd; //TODO make generator static
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        float alpha = dis(gen);
        return baseColor * (1 - alpha) + alpha * (*select_randomly(horizontalMixColors.begin(), horizontalMixColors.end()));
    }

    static glm::vec3 fc(glm::vec3 col)
    {
        return {col.x/255, col.y/255, col.z/255};
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
    colors
    {
            {//snow
                0.8,
                {255,255,255}, //white
                    std::vector<glm::vec3>{
                    {126, 249, 255},
                    {115,194,251},
                    {176, 223,229},
                    {149, 200, 216}
                },
                 0.3
            },
            {//mountain
                    0.3,
                    {110,116,120}, //Rockish
                    std::vector<glm::vec3>{
                        {181,170,157},
                        {65,71,74},
                        {0,0,0},
                        {129,132,125}
                    },
                    0.3
            },
            {//greenZone
                    -0.3,
                    {43,85,19},
                    std::vector<glm::vec3>{
                        {39,70,12},
                        {55,89,11},
                        {134,178,74},
                        {54,143,0},
                     },
                    0.1
            },
            {//underWater
                    -1,
                    {246,228,173},
                    std::vector<glm::vec3>{
                        {126, 249, 255},
                        {250,242,195},
                        {135,224,255},
                        {246,218,99},
                        {254,255,189}
                    },
                    0.2
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
        //addWater();
    }

    void HeightMapFromFile(const char* file)
    {

    };


    /**
     * Clears lod heightMap and generates new;
     * @param seed
     * @return
     */
    [[nodiscard]] float** generateHeightMap(int seed = 3245432)
    {
        clearHeightMap();

        FastNoise noise;
        noise.SetSeed(seed);
        noise.SetFractalOctaves(12);
        noise.SetNoiseType(FastNoise::SimplexFractal);
        noise.SetFrequency(.3);
        noise.SetFractalLacunarity(2);
        noise.SetFractalGain(0.5);

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

        for(unsigned int row = 0; row < height; ++row)
        {
            for(unsigned int col = 0; col < width; ++col)
            {
                unsigned topLeft = row * (width+1) + col;
                unsigned topRight = topLeft + 1;
                unsigned bottomLeft = topLeft + (width+1);
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
                                bottomRight,
                                topRight

                        });
            }
        }
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

        for(int j = 0; j <= height; ++j)
        {
            for(int i = 0; i <= width; ++i)
            {
                vertices.emplace_back(
                    Vertex(glm::vec3(1 - (2.f * i)/fwidth, heightMap[i*tileSize][j*tileSize], 1 - (2.f * j)/fheight),
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

    void addWater()
    {
        unsigned size = vertices.size();
        float h = (colors.end()-2)->bottomLine;
        float x = 1;
        float z = 1;
        glm::vec3 col = glm::vec3(81./255., 158./255., 173./255.);

        vertices.emplace_back(
                Vertex(glm::vec3(x, h, z),
                       glm::vec3(0,0,0 ),
                       glm::vec2( -1, -1)),
                       col);

        vertices.emplace_back(
                Vertex(glm::vec3(-x, h, z),
                       glm::vec3(0,0,0 ),
                       glm::vec2( -1, 1)),
                col);

        vertices.emplace_back(
                Vertex(glm::vec3(-x, h, -z),
                       glm::vec3(0,0,0 ),
                       glm::vec2( 1, 1)),
                col);

        vertices.emplace_back(
                Vertex(glm::vec3(x, h, -z),
                       glm::vec3(0,0,0 ),
                       glm::vec2( 1, -1)),
                col);


        indices.insert(indices.end(), {
            size, size + 2, size + 1,
            size, size + 3, size + 2
        });
    }

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
    std::vector<ZoneColor> colors;

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

    /**
     * Determine which color will be in fragment according to zone
     * Color is determined by Parabola equation, where current height stands for x, and alpha for y
     * y = x^2 + minimumAlpha
     * y - alpha, x - height
     * @param h Vertex height
     * @return Colour on that height
     */
    glm::vec3 determineColor(const float& h)
    {
        for( size_t i = 0; i < colors.size(); ++i )
        {
            if( h < colors[i].bottomLine )
                continue;

            float zoneCenter;
            float alpha;
            glm::vec3 color;

            if( !i )
                zoneCenter = 1;
            else
                if( i == colors.size() - 1 )
                    zoneCenter = -1;
                else
                    zoneCenter = (colors[i-1].bottomLine - colors[i].bottomLine)/2;

            alpha = std::pow(zoneCenter - h, 6) + colors[i].betweenLvlAlpha;

            if( h > zoneCenter )
                color = ZoneColor::fc((1.0f-alpha) * colors[i].mixHorizontalColor() + alpha * colors[i-1].mixHorizontalColor());
            else
                if( h < zoneCenter)
                    color = ZoneColor::fc((1.0f-alpha) * colors[i].mixHorizontalColor() + alpha * colors[i+1].mixHorizontalColor());
                else
                    color = ZoneColor::fc(colors[i].mixHorizontalColor());

            return color;
        }
        return {6,6,6};
    }

    unsigned int width; ///< in tiles
    unsigned int height; ///< in tiles

    unsigned int VBO{}, EBO{}, VAO{};

    float** heightMap = nullptr; ///< Do we really need to store it?
    unsigned tileSize = 15; ///< between verts

    /**
     * TODO: add vector of water lvls for this terrain
     */
};

#endif
