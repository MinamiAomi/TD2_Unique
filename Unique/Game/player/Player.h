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
#include "Reticle3D.h"
#include <array>

//振る舞い
enum class Behavior {
	kRoot, //通常状態
	kAttack, //攻撃中
	kShot, //重力発射
};

class Block;

class Player : public GameObject
{
public:
	Player();
	~Player();

	enum AttackType {
		kVertical, //縦攻撃
		kHorizontal_1, //横攻撃_1
		kHorizontal_2, //横攻撃_2
		kRotateAttack, //回転攻撃
	};

	//プレイヤーパーツのナンバリング
	enum PlayerParts {
		kHip, //腰
		kBody, //体
		kHead, //頭
		kLeftShoulder, //左肩
		kLeftUpperArm, //左腕上部
		kLeftLowerArm, //左腕下部
		kRightShoulder, //右肩
		kRightUpperArm, //右腕上部
		kRightLowerArm, //右腕下部
		kLeftUpperLeg, //左足上部
		kLeftLowerLeg, //左足下部
		kRightUpperLeg, //右足上部
		kRightLowerLeg, //右足下部
		kMaxParts, //最大パーツ数。配列の数を設定する時等に使用
	};

	void Initialize();

	void Update();

	Vector3 GetPosition() {
		return playerTransforms_[kBody]->worldMatrix.GetTranslate();
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

	void SetPosition(const Vector3& position) { playerTransforms_[kHip]->translate = position; }

	bool GetIsBreak() const { return isBreak_; }

	void Damage(uint32_t val);

	bool GetIsInvincible() const { return workInvincible_.isInvincible; }

	void SetCamera(std::shared_ptr<FollowCamera> camera) { 
		camera_ = camera;
		reticle_->SetCamera(camera_);
	}

	const std::shared_ptr<FollowCamera>& GetCamera() { return camera_; }

	std::shared_ptr<Reticle3D> GetReticle() { return reticle_; }

	std::array<std::shared_ptr<Transform>, kMaxParts> playerTransforms_;

	//死んだかどうか
	bool GetIsDead() const { return isDead_; }

private:

	void OnCollision(const CollisionInfo& collisionInfo);

	void Thrust();

	void RegisterGlobalVariables();

	void ApplyGlobalVariables();

private:

	Input* input_ = nullptr;

	std::shared_ptr<ModelInstance> playerModel_;

	std::array<std::shared_ptr<ModelInstance>, kMaxParts> playerModels_;

	std::shared_ptr<FollowCamera> camera_;

	std::unique_ptr<Weapon> weapon_;

	std::shared_ptr<Reticle3D> reticle_;

	//前フレームの位置
	Vector3 prePosition_{};

	//ダッシュ用ワーク
	struct WorkDash {
		//ダッシュのスピード
		float speed_ = 2.0f;
	};

	//攻撃全体で使う変数
	struct Attack {
		//攻撃時間の媒介変数
		int32_t attackTimer = 0;
		//攻撃方法
		AttackType attackType;
		//プレイヤーの回転保存
		Quaternion playerRotate;
		//コンボを繋げるか
		bool isCombo_ = false;
		//最大コンボ数
		const uint32_t kMaxCombo_ = 3;
		//現在のコンボ
		uint32_t currentCombo_ = 0;
	};

	//攻撃用ワーク(デフォ値)
	struct WorkAttack_01 {
		//攻撃前の振りを溜める時間
		int32_t preFrame = 0;
		//攻撃前の待ち時間
		int32_t waitFrameBefore = 0;
		//攻撃中の時間
		int32_t attackFrame = 10;
		//攻撃後の待ち時間
		int32_t waitFrameAfter = 0;
		//合計フレーム数
		int32_t allFrame = 40;
		//攻撃前のY回転量
		float preRotate = 1.57f;
		//攻撃時のY回転量
		float attackRotate = -3.14f;
	};

	//攻撃用ワーク(デフォ値)
	struct WorkAttack_02 {
		//攻撃中の時間
		int32_t attackFrame = 10;
		//攻撃後の待ち時間
		int32_t waitFrameAfter = 20;
		//合計フレーム数
		int32_t allFrame = 30;
		//攻撃時のY回転量
		float attackRotate = 3.14f;
	};

	//攻撃用ワーク(デフォ値)
	struct WorkAttack_03 {
		//プレイヤーの移動速度
		Vector3 velocity{};
		//攻撃中の時間
		int32_t attackFrame = 40;
		//ジャンプ時のフレーム
		int32_t jumpFrame = 20;
		//ジャンプ中待機時間
		int32_t waitFrameJump = 10;
		//落下中のフレーム
		int32_t fallFrame = 20;
		//攻撃後の待ち時間
		int32_t waitFrameAfter = 40;
		//攻撃後の衝撃波発生時間
		int32_t shockWaveFrame = 10;
		//合計フレーム数
		int32_t allFrame = 130;
		//攻撃時のY回転量
		float attackRotate = -3.14f;
	};

	//弾発射時のワーク
	struct WorkShot {
		//腕の回転制御フレーム
		int32_t shotTimer = 0;
		//使用フレーム数
		int32_t maxShotFrame = 60;
		//回転量
		float shotRotate = 2.32f;
	};

	//重力を構えるときのワーク
	struct WorkGravity {
		//重力の制御フレーム
		int32_t gravityTimer = 0;
		//重力を用意するまでにかかるフレーム
		int32_t gravityFrame = 15;
		//重力の持続時間
		int32_t keepTime = 120;
		//重力を使用している時の制御変数
		int32_t overHeatTimer = 0;
		//重力を使用しすぎた場合の制御フラグ
		bool isOverHeat = false;
		//重力を使用している時の減速量
		float decelVal = 0.2f;
		//実際に速度に影響を与える変数
		float decel = 0.0f;
	};

	//無敵状態ワーク
	struct WorkInvincible {
		//無敵中かどうか
		bool isInvincible;
		//無敵時間
		int32_t invincibleTimer;
	};

	WorkDash workDash_;

	Attack attack_;

	WorkAttack_01 WA_01_;

	WorkAttack_02 WA_02_;

	WorkAttack_03 WA_03_;

	WorkShot workShot_;

	WorkGravity workGravity_;

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

	//ボタンの長押し時間
	uint32_t inputTime_ = 0;

	//通常行動更新
	void BehaviorRootUpdate();

	//通常行動初期化
	void BehaviorRootInitialize();

	//攻撃行動更新
	void BehaviorAttackUpdate();

	//攻撃行動初期化
	void BehaviorAttackInitialize();

	//発射行動更新
	void BehaviorShotUpdate();

	//発射行動初期化
	void BehaviorShotInitialize();

	//死亡フラグ
	bool isDead_ = false;

	//発射準備中かどうか
	bool isPoseShot_ = false;

	//当たり判定(OBB)
	std::unique_ptr<BoxCollider> collider_;
 
	std::shared_ptr<Texture> hpTex_;
	std::shared_ptr<Texture> guardTex_;

	std::unique_ptr<Sprite> hpSprite_;
	std::unique_ptr<Sprite> guardSprite_;

	std::shared_ptr<Texture> ui_A_Tex_;
	std::shared_ptr<Texture> ui_RB_Tex_;
	std::shared_ptr<Texture> ui_LB_Tex_;
	std::shared_ptr<Texture> ui_RT_Tex_;
	std::shared_ptr<Texture> ui_RS_Tex_;
	std::shared_ptr<Texture> ui_LS_Tex_;


	std::unique_ptr<Sprite> ui_A_;
	std::unique_ptr<Sprite> ui_LB_;
	std::unique_ptr<Sprite> ui_RB_;
	std::shared_ptr<Sprite> ui_RT_;
	std::unique_ptr<Sprite> ui_LS_;
	std::unique_ptr<Sprite> ui_RS_;

	size_t dashSE_;
	size_t deathSE_;
	size_t shootSE_;
	size_t crashSE_;

};
