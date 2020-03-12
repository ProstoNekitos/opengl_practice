#include "particleGenerator.h"

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, GLuint amount, glm::vec3 pos
        , glm::vec3 dir)
        :
        shader(shader),
        texture(texture),
        amount(amount),
        position(pos),
        direction(dir)
{
    this->init();
}

void ParticleGenerator::update(float dt, unsigned int newParticles, glm::vec3 offset)
{
    // Add new particles
    for (unsigned int i = 0; i < newParticles; ++i)
    {
        unsigned int unusedParticle = firstUnusedParticle();
        this->respawnParticle(particles[unusedParticle], offset);
    }
    // update all particles
    for (unsigned int i = 0; i < amount; ++i)
    {
        Particle &p = this->particles[i];
        p.lifeTime -= dt; // reduce life
        if (p.lifeTime > 0.0f)
        {	// particle is alive, thus update
            p.position -= p.direction * dt;
            p.color.a -= dt * 2.5;
        }
    }
}

// Render all particles
void ParticleGenerator::render()
{
    // Use additive blending to give it a 'glow' effect
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    shader.use();

    for (Particle particle : this->particles)
    {
        if (particle.lifeTime > 0.0f)
        {
            shader.setVec3("offset", particle.position);
            shader.setVec4("color", particle.color);
            texture.Bind();

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init()
{
    // Set up mesh and attribute properties
    unsigned int VBO;
    float particle_quad[] = {
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,

            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // Create this->amount default particle instances
    for (unsigned int i = 0; i < amount; ++i)
        this->particles.emplace_back();
}

// Stores the index of the last particle used (for quick access to next dead particle)
GLuint lastUsedParticle = 0;
GLuint ParticleGenerator::firstUnusedParticle()
{
    // First search from last used particle, this will usually return almost instantly
    for (unsigned int i = lastUsedParticle; i < this->amount; ++i){
        if (this->particles[i].lifeTime <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // Otherwise, do a linear search
    for (unsigned int i = 0; i < lastUsedParticle; ++i){
        if (this->particles[i].lifeTime <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
    lastUsedParticle = 0;
    return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, glm::vec3 offset)
{
    float random = ((rand() % 100) - 50) / 10.0f;
    float rColor = 0.5 + ((rand() % 100) / 100.0f);
    particle.position = position + random + offset;
    particle.color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.lifeTime = 1.0f;
    particle.direction = direction * 0.1f;
}

void ParticleGenerator::setPosition(glm::vec3 pos) {
    position = pos;
}

