#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include "stb_image.h"

class Texture2D
{
public:

    Texture2D();
    void Generate(unsigned int w, unsigned int h, unsigned char* data);
    void Bind() const;

    unsigned int ID;
    unsigned int width, height; // Width and height of loaded image in pixels

    unsigned int Internal_Format; // Format of texture object
    unsigned int Image_Format; // Format of loaded image

    unsigned int Wrap_S; // Wrapping mode on S axis
    unsigned int Wrap_T; // Wrapping mode on T axis

    unsigned int Filter_Min; // Filtering mode if texture pixels < screen pixels
    unsigned int Filter_Max; // Filtering mode if texture pixels > screen pixels
};

#endif

