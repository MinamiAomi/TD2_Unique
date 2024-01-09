#pragma once

#include <list>

#include "Math/MathUtils.h"

struct Particle {
    Vector3 position;       // 位置
    Vector3 velocity;       // 速度
    float startSize;        // 開始時の大きさ
    float endSize;          // 終了時の大きさ
    Vector3 startColor;     // 開始時の色
    Vector3 endColor;       // 終了時の色
    float startAlpha;       // 開始時の透明度
    float endAlpha;         // 終了時の透明度
    uint32_t existenceTime; // 存在時間
    uint32_t lifeTime;      // 寿命
};

class ParticleManager {
public:
    static ParticleManager* GetInstance();

    void AddParticles(std::list<Particle>&& particle) { particles_.splice(particles_.end(), particle); }
    void Update();

    const std::list<Particle>& GetParticles() const { return particles_; }

private:
    ParticleManager() = default;
    ~ParticleManager() = default;
    ParticleManager(const ParticleManager&) = delete;
    ParticleManager& operator=(const ParticleManager&) = delete;

    std::list<Particle> particles_;
};
