#ifndef UNTITLED_LIGHT_H
#define UNTITLED_LIGHT_H

#include <glm/glm.hpp>

struct lightParts
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct lightDistanceParts
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
    explicit Light(LightType t, lightParts p = {glm::vec3(1.0f),glm::vec3(1.0f),glm::vec3(1.0f)})
    : parts(p), type(t){}

public:
    lightParts parts;
    LightType type;
};

class DirectionLight : virtual public Light
{
public:
    explicit DirectionLight(
            glm::vec3 dir = {0,0,0},
            lightParts p = {glm::vec3(1.0f),glm::vec3(1.0f),glm::vec3(1.0f)}
            ) :
            Light(LightType::DIRECTIONAL, p), direction(dir){}

    glm::vec3 direction;
};

class PointLight : virtual public Light
{
public:
    explicit PointLight(
            glm::vec3 pos = {0,0,0},
            lightDistanceParts dist = {1.0f, 0.09, 0.032},
            lightParts p = {glm::vec3(1.0f),glm::vec3(1.0f),glm::vec3(1.0f)}
    ) :
    Light(LightType::POINT, p), position(pos), distance(dist){}

    glm::vec3 position;
    lightDistanceParts distance;
};

/**
 * RUN, YOU, FOOLS
 */
class SpotLight : public virtual PointLight, virtual DirectionLight
{

public:
    explicit SpotLight(
            lightParts p = {glm::vec3(1.0f),glm::vec3(1.0f),glm::vec3(1.0f)},
            glm::vec3 pos = {0,0,0},
            lightDistanceParts dist = {1.0f, 0.09, 0.032},
            glm::vec3 dir = {0,0,0},
            float cut = .5,
            float oCut = 1
                    ) :
              PointLight(pos, dist),
              DirectionLight(dir),
              Light(LightType::SPOT, p),
              cutOff(cut),
              outerCutOff(oCut){}

    float cutOff;
    float outerCutOff;
};

#endif
