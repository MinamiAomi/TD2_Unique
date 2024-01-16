#pragma once
#include "Graphics/Model.h"
#include "Graphics/Sprite.h"
#include <memory>
#include "Game/block/block.h"
#include "Game/player/Player.h"
#include "Collision/Collider.h"
#include "Collision/GameObject.h"
#include "EnemyBullet.h"
#include "Audio/Audio.h"
#include <array>
#include "EnemyCore.h"

class Enemy : public GameObject
{
public:
	Enemy();
	~Enemy();

	void Initialize();

	void Update();

	/*void SetBlockList(std::list<std::shared_ptr<Block>>* ptr) { blocksPtr_ = ptr; }*/

	void SetPlayer(Player* player) { player_ = player; }

	bool GetIsDead() const { return isDead_; }

	void Damage(int32_t val) { hp_ -= val; }

	const std::list<std::shared_ptr<EnemyBullet>>& GetBullets() { return bullets_; }

	const std::array<std::shared_ptr<EnemyCore>, 8>& GetCores() { return enemyCores_; }

private:

	void Attack();

	void AttackInitialize();

	void AddBullet();

	void OnCollision(const CollisionInfo& collisionInfo);

	void ResetCores();

	//初期位置に戻す
	void SetCoresToRoot();

	int32_t CalcAllHp();

private:

	enum CorePosition {
		kLeftDownFront,//左下前
		kLeftDownBack,//左下奥
		kLeftTopFront,//左上前
		kLeftTopBack,//左上奥
		kRightDownFront,//右下前
		kRightDownBack,//右下奥
		kRightTopFront,//右上前
		kRightTopBack,//右上奥
	};

	struct WorkAttack
	{
		uint32_t attackCount; //攻撃回数(ブロックを出す数)
		uint32_t startAttackInterval; //攻撃中のフレーム数
		int32_t startAttackTimer; //攻撃中のカウント
	};

	struct WorkShot
	{
		uint32_t shotInterval; //発射間隔
		uint32_t shotCount; //弾を発射する数
		int32_t shotTimer; //弾の発射管理をするタイマー
	};

	struct CrossAttack {
		//std::array<std::shared_ptr<ModelInstance>, 2> models_; //攻撃モデル
		//std::array<std::unique_ptr<BoxCollider>, 2> colliders_; //攻撃コライダー
		//Transform transforms[2]; //トランスフォーム
		uint32_t attackInterval = 60; //発射までの猶予
		Vector3 shotPosition[2]; //発射する位置
		uint32_t maxAttackTime = 180; //攻撃に使う時間
		int32_t attackTimer; //攻撃管理タイマー
	};

	uint32_t attackInterval_; //攻撃間隔
	int32_t attackTimer_; //攻撃するまでのカウント
	bool isStartAttack_; //攻撃開始フラグ

	uint32_t attackNumber_ = 0;

	//当たり判定
	/*std::unique_ptr<BoxCollider> collider_;*/

	/*std::shared_ptr<ModelInstance> model_;*/

	//攻撃場所を表示する用のモデル
	std::array<std::shared_ptr<ModelInstance>, 10> attackModels_;

	std::array<Transform, 10> attackTransforms_;

	//敵のコア
	std::array<std::shared_ptr<EnemyCore>, 8> enemyCores_;

	Player* player_ = nullptr;

	//ブロックリストのポインタ。ブロックを追加するのに使用
	/*std::list<std::shared_ptr<Block>>* blocksPtr_ = nullptr;*/

	//敵弾
	std::list<std::shared_ptr<EnemyBullet>> bullets_;

	std::list<std::shared_ptr<EnemyBullet>> bigBullets_;

	WorkAttack workAttack_;

	WorkShot workShot_;

	CrossAttack crossAttack_;

	//速度
	Vector3 velocity_{};

	uint32_t kMaxHp_ = 40;

	int32_t hp_ = kMaxHp_;

	//1hp辺りの長さ
	float hpWidth_ = float(300.0f / kMaxHp_);

	bool isDead_ = false;

	//移動先の位置
	Vector3 movePosition_{};

	//攻撃する位置を格納する配列
	std::array<Vector3, 10> attackPositions_;
	//攻撃のサイズ
	std::array<Vector3, 10> attackSizes_;
	//攻撃の当たり判定
	std::array<std::unique_ptr<BoxCollider>, 10> attackColliders_;

	std::shared_ptr<Texture> hpTex_;

	std::unique_ptr<Sprite> hpSprite_;

	size_t groundAttackSE_;
	size_t shotSE_;
	size_t deathSE_;

	uint32_t invincibleTime_ = 30;

	int32_t hitCoolTime_ = 0;

};
