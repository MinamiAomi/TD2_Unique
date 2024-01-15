#include "Enemy.h"
#include "Math/Random.h"
#include "Graphics/ResourceManager.h"

static Random::RandomNumberGenerator randomNumberGenerator;

Enemy::Enemy()
{
	model_ = std::make_shared<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("Cube"));

	for (uint32_t i = 0; i < 10; i++) {
		attackModels_[i] = std::make_shared<ModelInstance>();
		attackModels_[i]->SetModel(ResourceManager::GetInstance()->FindModel("Cube"));
		attackModels_[i]->SetIsActive(false);
	}

	hpTex_ = ResourceManager::GetInstance()->FindTexture("enemy_hp");

	hpSprite_ = std::make_unique<Sprite>();
	hpSprite_->SetTexture(hpTex_);

	collider_ = std::make_unique<BoxCollider>();

}

Enemy::~Enemy()
{
}

void Enemy::Initialize() {

	SetName("Enemy");

	transform.translate = { 0.0f,5.0f,50.0f };
	transform.scale = { 10.0f,10.0f,10.0f };
	transform.rotate = Quaternion::identity;

	collider_->SetCenter(transform.translate);
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetOrientation(transform.rotate);
	collider_->SetName("Enemy");
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) {OnCollision(collisionInfo); });

	hp_ = kMaxHp_;
	isDead_ = false;

	attackInterval_ = 150;
	workAttack_.attackCount = 3;
	attackTimer_ = attackInterval_;
	isStartAttack_ = false;
	workAttack_.startAttackInterval = 90;
	workAttack_.startAttackTimer = 0;

	workShot_.shotInterval = 180;
	workShot_.shotCount = 10;
	workShot_.shotTimer = workShot_.shotInterval;

	hpSprite_->SetPosition({ 640.0f,600.0f });
	hpSprite_->SetScale({ hpWidth_ * hp_, 64.0f });

	bullets_.clear();

	groundAttackSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/proto_sound/boss_groundAttack.wav");
	shotSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/proto_sound/boss_shot.wav");
	deathSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/proto_sound/disolve.wav");

}

void Enemy::Update() {

	bullets_.remove_if([](auto& bullet) {

		if (bullet->GetIsDead()) {
			return true;
		}

		return false;

		});

	if (!isDead_) {

		if (isStartAttack_) {
			Attack();
		}
		else {

			//タイマー0で攻撃時の変数初期化、攻撃開始
			if (--attackTimer_ <= 0) {
				
				//壁生やし以外の攻撃だったら壁生やしを行う
				attackNumber_ = 1;

				AttackInitialize();
				attackTimer_ = attackInterval_;
				isStartAttack_ = true;

			}

		}

		for (auto& bullet : bullets_) {
			bullet->Update();
		}

		if (hp_ <= 0) {
			isDead_ = true;
			Audio::GetInstance()->SoundPlayWave(deathSE_);
		}

		

	}

	hpSprite_->SetScale({ hpWidth_ * hp_, 64.0f });

	transform.UpdateMatrix();
	model_->SetWorldMatrix(transform.worldMatrix);

	collider_->SetCenter(transform.translate);
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetOrientation(transform.rotate);

	if (hitCoolTime_ > 0) {
		hitCoolTime_--;
	}

	//死んだとき
	if (isDead_) {

		model_->SetIsActive(false);

	}
	//生きている時
	else {

		if (hitCoolTime_ % 2 == 0) {
			model_->SetIsActive(true);
		}
		else {
			model_->SetIsActive(false);
		}
		
	}

	//地面攻撃中
	if (isStartAttack_ && attackNumber_ == 0) {

		for (uint32_t i = 0; i < workAttack_.attackCount; i++) {
			attackModels_[i]->SetIsActive(true);
		}

	}
	else {

		for (uint32_t i = 0; i < workAttack_.attackCount; i++) {
			attackModels_[i]->SetIsActive(false);
		}

	}

}

void Enemy::Attack() {

	switch (attackNumber_)
	{
	default:
	case 0:

		//カウントが30を切ったら攻撃開始
		if (--workAttack_.startAttackTimer < 30) {

			for (uint32_t i = 0; i < workAttack_.attackCount; i++) {

				attackTransforms_[i].translate.y += attackSizes_[i].y * 2.0f / 30.0f;
				attackTransforms_[i].UpdateMatrix();
				attackColliders_[i]->SetCenter(attackTransforms_[i].translate);
				attackModels_[i]->SetWorldMatrix(attackTransforms_[i].worldMatrix);

				/*if (OnCollision(attackColliders_[i], player_->GetCollision())) {
					player_->Damage(3);
				}*/

			}

			if (workAttack_.startAttackTimer <= 0) {

				if (blocksPtr_ && isStartAttack_) {

					for (uint32_t i = 0; i < workAttack_.attackCount; i++) {
						std::shared_ptr<Block> block = std::make_shared<Block>();
						block->Initialize(attackTransforms_[i].translate,
							player_, attackSizes_[i]);
						blocksPtr_->push_back(block);
					}

				}

				isStartAttack_ = false;

			}

		}
		else {

			if (workAttack_.startAttackTimer == 30) {
				Audio::GetInstance()->SoundPlayWave(groundAttackSE_);
			}

		}

		break;
	case 1:

		for (auto& bullet : bullets_) {

			if (workShot_.shotTimer % 10 == 0 && workShot_.shotTimer <= 120 && !bullet->GetIsShot()) {
				bullet->Shot(player_->GetPosition());
				Audio::GetInstance()->SoundPlayWave(shotSE_);
				break;
			}

		}

		if (--workShot_.shotTimer <= 0) {
			isStartAttack_ = false;
		}

		break;
	}

}

void Enemy::AttackInitialize() {

	switch (attackNumber_)
	{
	default:
	case 0:

		workAttack_.attackCount = randomNumberGenerator.NextIntRange(3, 6);

		for (uint32_t i = 0; i < workAttack_.attackCount; i++) {
			attackSizes_[i] = { float(randomNumberGenerator.NextIntRange(1,5)), float(randomNumberGenerator.NextIntRange(2,5)),
				float(randomNumberGenerator.NextIntRange(1,5)) };
			attackPositions_[i] = Vector3{ float(randomNumberGenerator.NextIntRange(-30,30)),
				attackSizes_[i].y, float(randomNumberGenerator.NextIntRange(-30,30)) };
			attackTransforms_[i].scale = attackSizes_[i];
			attackTransforms_[i].translate = player_->GetPosition() + attackPositions_[i] - Vector3{ 0.0f, attackPositions_[i].y * 2.0f + 0.1f, 0.0f };
			attackColliders_[i]->SetCenter(attackTransforms_[i].translate);
			attackColliders_[i]->SetSize(attackTransforms_[i].scale);
			attackColliders_[i]->SetName("Enemy_Block");
			attackTransforms_[i].UpdateMatrix();
			attackModels_[i]->SetWorldMatrix(attackTransforms_[i].worldMatrix);
		}

		isStartAttack_ = true;
		attackTimer_ = attackInterval_;
		workAttack_.startAttackTimer = workAttack_.startAttackInterval;

		break;
	case 1:

		AddBullet();
		workShot_.shotTimer = workShot_.shotInterval;

		break;
	}

}

//敵のヒット処理
void Enemy::OnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Weapon") {

		if (hitCoolTime_ == 0) {
			hp_--;
			hitCoolTime_ = invincibleTime_;
		}

	}
	else if (collisionInfo.collider->GetName() == "Block") {

		if (hitCoolTime_ == 0) {
			hp_ -= 3;
			hitCoolTime_ = invincibleTime_;
		}

	}

}

//ブロック攻撃のヒット処理
void Enemy::OnBlockAttackCollision(const CollisionInfo& collisionInfo) {

	collisionInfo;

}

void Enemy::AddBullet() {

	for (uint32_t i = 0; i < workShot_.shotCount; i++) {

		std::shared_ptr<EnemyBullet> newBullet = std::make_shared<EnemyBullet>();

		if (i < 5) {
			newBullet->Initialize(transform.translate + Vector3{(5.0f + i) * 5.0f,
				randomNumberGenerator.NextFloatRange(-5.0f,5.0f), randomNumberGenerator.NextFloatRange(-5.0f,5.0f) });
		}
		else {
			newBullet->Initialize(transform.translate + Vector3{i * -5.0f,
				randomNumberGenerator.NextFloatRange(-5.0f,5.0f), randomNumberGenerator.NextFloatRange(-5.0f,5.0f) });
		}

		bullets_.push_back(newBullet);

	}

}
