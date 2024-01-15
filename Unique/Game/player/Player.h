#pragma once
#include "Graphics/Model.h"
#include <optional>
#include "input/Input.h"
#include "Graphics/Sprite.h"
#include "Collision/Collider.h"
#include "Audio/Audio.h"
#include "Collision/GameObject.h"
#include "Game/followCamera/FollowCamera.h"
#include "Game/player/Weapon.h"

//振る舞い
enum class Behavior {
	kRoot, //通常状態
	kAttack, //攻撃中
	kDash, //ダッシュ
};

class Player : public GameObject
{
public:
	Player();
	~Player();

	enum AttackType {
		kVertical, //縦攻撃
		kHorizontal //横攻撃
	};

	void Initialize();

	void Update();

	const Vector3& GetLocalPosition() { return transform.translate; }

	Vector3 GetPosition() {
		return Vector3{ 
			transform.worldMatrix.m[3][0],
			transform.worldMatrix.m[3][1] ,
			transform.worldMatrix.m[3][2] };
	}

	Vector3 GetPrePosition() const {
		return prePosition_;
	}

	Vector3 GetDirection() const {
		return direction_;
	}

	Vector3 GetAttackPosition() {
		return weapon_->GetPosition();
	}

	Vector3 GetNewBlockPosition() {
		return weapon_->GetPosition() + (direction_ * 3.5f);
	}

	void SetPosition(const Vector3& position) { transform.translate = position; }

	bool GetIsBreak() const { return isBreak_; }

	bool GetIsAttack() const { return workAttack_.isAttack; }

	bool GetIsHit() const { return workAttack_.isHit; }

	void SetIsHit(bool flag) { workAttack_.isHit = flag; }

	const AttackType& GetAttackType() { return workAttack_.attackType; }

	void Damage(uint32_t val);

	bool GetIsInvincible() const { return workInvincible_.isInvincible; }

	void SetCamera(std::shared_ptr<FollowCamera> camera) { camera_ = camera; }

private:

	void OnCollision(const CollisionInfo& collisionInfo);

private:

	Input* input_ = nullptr;

	std::shared_ptr<ModelInstance> playerModel_;

	std::shared_ptr<FollowCamera> camera_;

	std::unique_ptr<Weapon> weapon_;

	//前フレームの位置
	Vector3 prePosition_{};

	//ダッシュ用ワーク
	struct WorkDash {
		//ダッシュ用の媒介変数
		uint32_t dashParamater_ = 0;
		//ダッシュのスピード
		float speed_ = 2.0f;
		//ダッシュ時間
		int32_t dashTime_ = 5;
	};

	//攻撃用ワーク
	struct WorkAttack {
		//速度
		Vector3 velocity;
		//攻撃時間の媒介変数
		int32_t attackTimer;
		//攻撃後の待ち時間
		int32_t waitFrame = 10;
		//攻撃フレーム数
		int32_t attackFrame = 20;
		//攻撃タイプ
		AttackType attackType;
		//攻撃中かどうか
		bool isAttack = false;
		//攻撃が当たったかどうか
		bool isHit = false;
	};

	//無敵状態ワーク
	struct WorkInvincible {
		//無敵中かどうか
		bool isInvincible;
		//無敵時間
		int32_t invincibleTimer;
	};

	WorkDash workDash_;

	WorkAttack workAttack_;

	WorkInvincible workInvincible_;

	uint32_t kMaxHp_ = 10;

	int32_t hp_;

	//振る舞い
	Behavior behavior_ = Behavior::kRoot;

	//次の振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	//前回向いていた向き
	Vector3 preDirection_{};

	//今向いている向き
	Vector3 direction_{};

	//回転行列
	Matrix4x4 directionToDirection_{};

	//移動速度
	Vector3 velocity_{};

	//ブロック破壊フラグ
	bool isBreak_ = false;

	//通常行動更新
	void BehaviorRootUpdate();

	//通常行動初期化
	void BehaviorRootInitialize();

	//攻撃行動更新
	void BehaviorAttackUpdate();

	//攻撃行動初期化
	void BehaviorAttackInitialize();

	//ダッシュ更新
	void BehaviorDashUpdate();

	//ダッシュ初期化
	void BehaviorDashInitialize();

	//攻撃
	void Attack();

	//死亡フラグ
	bool isDead_ = false;

	//当たり判定(球)
	std::unique_ptr<BoxCollider> collider_;

	std::shared_ptr<Texture> hpTex_;

	std::unique_ptr<Sprite> hpSprite_;

	std::shared_ptr<Texture> ui_A_Tex_;
	std::shared_ptr<Texture> ui_RB_Tex_;
	std::shared_ptr<Texture> ui_LB_Tex_;

	std::unique_ptr<Sprite> ui_A_;
	std::unique_ptr<Sprite> ui_LB_;
	std::unique_ptr<Sprite> ui_RB_;

	size_t dashSE_;
	size_t deathSE_;
	size_t shootSE_;
	size_t crashSE_;

};
