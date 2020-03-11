#ifndef UNTITLED_LIGHT_H
#define UNTITLED_LIGHT_H

#include <glm/glm.hpp>

struct lightComponents
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct lightDistanceComponents
{
    float constant;
    float linear;
    float quadratic;
};

enum LightType: int{ DIRECTIONAL, POINT, SPOT };

/**
 * Virtual base class
 */
class Light
{
protected:
    explicit Light(LightType t, lightComponents p = {glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f)})
    : parts(p), type(t){}

    virtual ~Light()= default;;
public:
    lightComponents parts;
    LightType type;

    bool active = true;
};

class DirectionalLight : virtual public Light
{
public:
    explicit DirectionalLight(
            glm::vec3 dir = {0,0,0},
            lightComponents p = {glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f)}
            ) :
            Light(LightType::DIRECTIONAL, p), direction(dir){}

    glm::vec3 direction;
};

class PointLight : virtual public Light
{
public:
    explicit PointLight(
            glm::vec3 pos = {0,0,0},
            lightDistanceComponents dist = {1.0f, 0.09, 0.032},
            lightComponents p = {glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f)}
    ) :
    Light(LightType::POINT, p), position(pos), distance(dist){}

    glm::vec3 position;
    lightDistanceComponents distance;
};

/**
 * RUN, YOU, FOOLS
 */
class SpotLight : public PointLight, public DirectionalLight
{

public:
    explicit SpotLight(
            lightComponents p = {glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f)},
            glm::vec3 pos = {0,0,0},
            lightDistanceComponents dist = {1.0f, 0.09, 0.032},
            glm::vec3 dir = {0,0,0},
            float cut = .5,
            float oCut = 1
                    ) :
            PointLight(pos, dist),
            DirectionalLight(dir),
            Light(LightType::SPOT, p),
            cutOff(cut),
            outerCutOff(oCut){}

            /**
             * Measured in degrees
             */
    float cutOff;
    float outerCutOff;
};

#endif
