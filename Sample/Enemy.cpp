#include "Enemy.h"

#include "Graphics/ResourceManager.h"
#include "Graphics/ParticleManager.h"
#include "Math/Random.h"
#include "Math/Color.h"

static Random::RandomNumberGenerator randomNumberGenerator;

struct PartInit {
    Vector3 translate;
    Quaternion rotate;
};

PartInit partInits[] = {
    {{0.0f, 1.0f, 0.0f},{}},
    {{0.0f, 1.0f, 0.0f},{}}
};

void Enemy::Initialize(const Vector3& basePosition) {
    SetName("Enemy");

    transform.translate = basePosition + Vector3{ 1.5f, 0.0f, 0.0f };
    transform.rotate = Quaternion::identity;
    transform.scale = Vector3::one * 0.5f;

    const char* partModelName[] = {
        "MimicBody",
        "MimicHead"
    };
    ResourceManager* resourceManager = ResourceManager::GetInstance();

    for (size_t i = 0; i < static_cast<size_t>(Part::NumParts); ++i) {
        parts_[i] = std::make_unique<PartData>();
        parts_[i]->model.SetModel(resourceManager->FindModel(partModelName[i]));
        parts_[i]->model.SetUseOutline(false);
        parts_[i]->transform.SetParent(&transform);
        parts_[i]->transform.translate = partInits[i].translate;
        parts_[i]->transform.rotate = partInits[i].rotate;
        parts_[i]->transform.UpdateMatrix();
    }

    collider_ = std::make_unique<BoxCollider>();
    collider_->SetGameObject(this);
    collider_->SetName("Enemy");
    collider_->SetSize(Vector3::one);
    collider_->SetCallback([this](const CollisionInfo& collisionInfo) {OnCollision(collisionInfo); });

    isDead_ = false;
    alpha_ = 1.0f;
    life_ = 3;
    invincibleTime_ = 15;
    hitCoolTime_ = 0;
}

void Enemy::Update() {

    // 死亡時演出
    if (isDead_) {
        DeadUpdate();
    }
    else {
        AliveUpdate();
        UpdateAnimation();
    }

    transform.UpdateMatrix();


    for (size_t i = 0; i < static_cast<size_t>(Part::NumParts); ++i) {
        parts_[i]->transform.UpdateMatrix();
        parts_[i]->model.SetWorldMatrix(parts_[i]->transform.worldMatrix);
    }

    collider_->SetCenter(Vector3{ 0.0f,0.5f,0.5f } * transform.worldMatrix);
    collider_->SetOrientation(transform.rotate);
}

Vector3 Enemy::GetCenter() {
    const Vector3 offset = { 0.0f, 0.5f, 0.5f };
    return offset * transform.worldMatrix;
}

void Enemy::AliveUpdate() {
    if (hitCoolTime_ > 0) {
        --hitCoolTime_;
    }

    transform.rotate = Quaternion::MakeForYAxis(-4.0f * Math::ToRadian) * transform.rotate;

    const float moveSpeed = 0.1f;
    Vector3 move = { 0.0f,0.0f,1.0f };
    move = transform.rotate * move * moveSpeed;
    transform.translate += move;
}

void Enemy::DeadUpdate() {
    transform.rotate = Quaternion::MakeForYAxis(-10.0f * Math::ToRadian) * transform.rotate;

    if (alpha_ > 0.0f) {
        alpha_ -= 1.0f / 120.0f;
        transform.translate += leapingVelocity_ * 0.1f;
        for (size_t i = 0; i < static_cast<size_t>(Part::NumParts); ++i) {
            parts_[i]->model.SetAlpha(alpha_);
        }
    }
    else {
        // 一度だけ
        if (alpha_ > -0.5f) {
            DeadEffect();
            alpha_ = -1.0f;
        }
    }
}

void Enemy::UpdateAnimation() {
    // 開けて閉じる周期
    const uint16_t cycle = 240;
    // 1フレーム変化量
    const float delta = Math::TwoPi / cycle;

    animationParameter_ += delta;
    animationParameter_ = std::fmod(animationParameter_, Math::TwoPi);

    // 開閉最大角
    const Quaternion openLimitRotate = Quaternion::MakeForXAxis(-45.0f * Math::ToRadian);

    animationParameter_ += delta;
    float triangleWave = std::abs(std::sin(animationParameter_));
    parts_[static_cast<size_t>(Part::Head)]->transform.rotate = Quaternion::Slerp(triangleWave, Quaternion::identity, openLimitRotate);
}

void Enemy::OnCollision(const CollisionInfo& collisionInfo) {
    if (collisionInfo.collider->GetName() == "Weapon") {

        if (hitCoolTime_ == 0) {
            --life_;
            hitCoolTime_ = invincibleTime_;
            HitEffect();
        }

        if (life_ == 0) {
            isDead_ = true;
            collider_->SetIsActive(false);
            alpha_ = 1.0f;
            leapingVelocity_ = collisionInfo.normal;
        }
    }
}

void Enemy::HitEffect() {

    const uint32_t numParticles = 30;
    std::list<Particle> particles(numParticles);

    Vector3 base = transform.worldMatrix.GetTranslate();

    for (auto& particle : particles) {
        particle.position = base;

        particle.velocity.x = randomNumberGenerator.NextFloatRange(-1.0f, 1.0f);
        particle.velocity.y = randomNumberGenerator.NextFloatRange(0.0f, 1.0f);
        particle.velocity.z = randomNumberGenerator.NextFloatRange(-1.0f, 1.0f);
        particle.velocity = particle.velocity.Normalized() * 0.2f;

        particle.startSize = 0.5f;
        particle.endSize = 0.1f;
        particle.startColor = Vector3::unitX;
        particle.endColor = Vector3::unitX;
        particle.startAlpha = 1.0f;
        particle.endAlpha = 0.0f;
        particle.existenceTime = 0;
        particle.lifeTime = 10;
    }

    ParticleManager::GetInstance()->AddParticles(std::move(particles));
}

void Enemy::DeadEffect() {
    const uint32_t numParticles = 50;
    std::list<Particle> particles(numParticles);

    Vector3 base = transform.worldMatrix.GetTranslate();

    for (auto& particle : particles) {
        particle.position = base;

        particle.velocity.x = randomNumberGenerator.NextFloatRange(-1.0f, 1.0f);
        particle.velocity.y = randomNumberGenerator.NextFloatRange(-1.0f, 1.0f);
        particle.velocity.z = randomNumberGenerator.NextFloatRange(-1.0f, 1.0f);
        particle.velocity = particle.velocity.Normalized() * 0.2f;

        particle.startSize = 0.5f;
        particle.endSize = 0.1f;
        Color color = Color::HSVA(randomNumberGenerator.NextFloatRange(0.0f, 1.0f), 1.0f, 1.0f);
        particle.startColor = color;
        particle.endColor = color;
        particle.startAlpha = 1.0f;
        particle.endAlpha = 0.0f;
        particle.existenceTime = 0;
        particle.lifeTime = 15;
    }

    ParticleManager::GetInstance()->AddParticles(std::move(particles));
}
