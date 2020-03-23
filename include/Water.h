#ifndef UNTITLED_WATER_H
#define UNTITLED_WATER_H

#include "Mesh.h"

/**
 * //TODO
 * Well, currently, i'm just following some guide, but i think, that it's possible to do refraction through alpha
 */

class Reflections
{
public:
    explicit Reflections(unsigned reflW = 500, unsigned reflH = 500, unsigned refrW = 500, unsigned refrH = 500) :
            reflWidth(reflW), reflHeight(reflH), refrWidth(refrW), refrHeight(refrH)

    {
        reflFBO = FBO();
        reflTexture = textureBuffer(reflW, reflH);
        reflDepth = depthBuffer(reflW, reflH);

        refrFBO = FBO();
        refrTexture = textureBuffer(refrW, refrH);
        refrDepthTexture = depthTextureBuffer(refrW, refrH);
    }

    unsigned getReflectionTexture()
    {
        return reflTexture;
    }

    unsigned getRefractionTexture()
    {
        return refrTexture;
    }

    unsigned bindReflection()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, reflFBO);
        glViewport(0, 0, reflWidth, reflWidth);
    }

    unsigned bindRefraction()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, refrFBO);
        glViewport(0, 0, refrWidth, refrWidth);
    }


private:
    unsigned FBO()
    {
        unsigned FBO;

        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);

        return FBO;
    }

    unsigned textureBuffer(unsigned w, unsigned h)
    {
        unsigned texBuff;

        glGenTextures(1, &texBuff);
        glBindTexture(GL_TEXTURE_2D, texBuff);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texBuff, 0);

        return  texBuff;
    }

    unsigned depthTextureBuffer(unsigned w, unsigned h)
    {
        unsigned reflDepthTexture;

        glGenTextures(1, &reflDepthTexture);
        glBindTexture(GL_TEXTURE_2D, reflDepthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, reflDepthTexture, 0);

        return reflDepthTexture;
    }

    unsigned depthBuffer(unsigned w, unsigned h)
    {
        unsigned depthBuffer;

        glGenRenderbuffers(1, &depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

        return depthBuffer;
    }

    //Reflection
    unsigned int reflFBO;
    unsigned reflTexture, reflDepth;
    unsigned reflWidth, reflHeight;

    //Refraction
    unsigned int refrFBO;
    unsigned refrTexture, refrDepthTexture;
    unsigned refrWidth, refrHeight; ///< Reflections resolution
};

class Water{
    float lvl;
    float muddy;
    glm::vec3 color;
};

#endif //UNTITLED_WATER_H
