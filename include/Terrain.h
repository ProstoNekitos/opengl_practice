#ifndef UNTITLED_TERRAIN_H
#define UNTITLED_TERRAIN_H

class Terrain
{
public:
    Terrain();
    void HeightMapFromFile(const char* file);
    void generateHeightMap(/*something like seed would be great*/);

    void setup();

    void render();
private:
    unsigned int width; ///< In vert number
    unsigned int height; ///< In vert number

    float offset;

    float seaLvl; ///< Idk where to use it except for coloring
};

#endif
