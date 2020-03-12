#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include "stb_image.h"
#include <string>

class Texture2D
{
public:

    enum TYPE: int {DIFFUSE, SPECULAR, NORMAL, HEIGHT};

    Texture2D();

    void generate(unsigned int w, unsigned int h, unsigned char* data);
    void bind() const;

    unsigned int ID{};
    unsigned int width, height; // Width and height of loaded image in pixels

    unsigned int internalFormat; // Format of texture object
    unsigned int textureFormat; // Format of loaded image

    unsigned int wrapModeS; // Wrapping mode on S axis
    unsigned int wrapModeT; // Wrapping mode on T axis

    unsigned int filterMin; // Filtering mode if texture pixels < screen pixels
    unsigned int filterMax; // Filtering mode if texture pixels > screen pixels

    TYPE type = TYPE::DIFFUSE;
};

#endif

