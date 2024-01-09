#include "ParticleManager.h"

ParticleManager* ParticleManager::GetInstance() {
    static ParticleManager instance;
    return &instance;
}

void ParticleManager::Update() {
    std::erase_if(particles_, 
        [](Particle& particle) { 
            particle.position += particle.velocity;
            ++particle.existenceTime;
            return particle.existenceTime >= particle.lifeTime; 
        });
}
