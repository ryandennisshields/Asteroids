//
//  This tutorial was used:
//  https://learnopengl.com/In-Practice/2D-Game/Particles
//
#include <vector>

//#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "texture.h"
#include "GameObject.h"


// Represents a single particle and its state
struct Particle {
    glm::vec3 Position, Velocity;
    glm::vec4 Color;
    float     Life;

    Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) {}
};


// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleManager
{
public:
    // constructor
    ParticleManager(Shader shader, Texture texture, unsigned int amount);
    // update all particles
    void Update(float dt, GameObject& object, unsigned int newParticles, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
    // render all particles
    void Draw();
private:
    // state
    std::vector<Particle> particles;
    unsigned int amount;
    // render state
    Shader shader;
    Texture texture;
    unsigned int VAO;
    // initializes buffer and vertex attributes
    void init();
    // returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    unsigned int firstUnusedParticle();
    // respawns particle
    void respawnParticle(Particle& particle, GameObject& object, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
};