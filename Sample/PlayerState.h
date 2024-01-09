#pragma once

#include <memory>
#include <array>

class Player;
class PlayerState;
struct CollisionInfo;

class PlayerStateManager {
public:
    PlayerStateManager(Player& player) : player(player) {}
    void Update();
    void OnCollision(const CollisionInfo& collisionInfo);

    template<class T>
    void ChangeState() {
        static_assert(std::is_base_of_v<PlayerState, T>, "Not inherited.");
        standbyState_ = std::make_unique<T>(*this);
    }

    Player& player;

private:
    std::unique_ptr<PlayerState> activeState_;
    std::unique_ptr<PlayerState> standbyState_;
};

class PlayerState {
public:
    PlayerState(PlayerStateManager& manager) : manager_(manager) {}
    virtual ~PlayerState() {}
    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void OnCollision(const CollisionInfo& collisionInfo) = 0;

protected:
    PlayerStateManager& manager_;
};

class PlayerStateRoot :
    public PlayerState {
public:
    using PlayerState::PlayerState;
    void Initialize() override;
    void Update() override;
    void OnCollision(const CollisionInfo& collisionInfo) override;

private:
    float ySpeed_;
    bool canJump_;
};

class PlayerStateAttack :
    public PlayerState {
public:
    using PlayerState::PlayerState;
    void Initialize() override;
    void Update() override;
    void OnCollision(const CollisionInfo& collisionInfo) override;

private:
    uint32_t attackParameter_;
    uint32_t comboIndex_;
    bool comboNext_;
};

class PlayerStateDush :
    public PlayerState {
public:
    using PlayerState::PlayerState;
    void Initialize() override;
    void Update() override;
    void OnCollision(const CollisionInfo& collisionInfo) override;

private:
    uint32_t dushParameter_;

};