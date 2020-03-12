#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture.h"

// Represents a single particle and its state
struct Particle {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec4 color;
    float lifeTime;

    Particle() : position(0.0f), direction(0.0f), color(1.0f), lifeTime(1.0f) { }
};


class ParticleGenerator
{
public:
    // Constructor
    ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount,
            glm::vec3 pos, glm::vec3 dir);
    // update all particles
    void update(GLfloat dt, unsigned int newParticles, glm::vec3 offset = glm::vec3(0.0f));
    // Render all particles
    void render();

    void setPosition(glm::vec3 pos);

private:
    std::vector<Particle> particles;
    unsigned int amount;
    Shader shader;
    Texture2D texture;
    unsigned int VAO;
    void init();
    unsigned int firstUnusedParticle();
    void respawnParticle(Particle &particle, glm::vec3 offset = glm::vec3(0.0f));

    glm::vec3 position;
    glm::vec3 direction;
};

#endif

