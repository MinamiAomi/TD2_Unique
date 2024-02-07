#include "Enemy.h"
#include "Math/Random.h"
#include "Graphics/ResourceManager.h"
#include "EnemyCoreManager.h"
#include "BulletManager.h"
#include "BarrierBulletManager.h"
#include "BulletManager.h"
#include "BarrierBulletManager.h"

static Random::RandomNumberGenerator randomNumberGenerator;

Enemy::Enemy()
{
	/*model_ = std::make_shared<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("Cube"));*/

	enemyCore_ = std::make_shared<EnemyCore>();

	hpTex_ = ResourceManager::GetInstance()->FindTexture("enemy_hp");
	hpOverTex_ = ResourceManager::GetInstance()->FindTexture("enemy_hp_over");

	hpSprite_ = std::make_unique<Sprite>();
	hpSprite_->SetTexture(hpTex_);
	hpOverSprite_ = std::make_unique<Sprite>();
	hpOverSprite_->SetTexture(hpOverTex_);

}

Enemy::~Enemy()
{
	enemyCore_;
}

void Enemy::Initialize() {

	SetName("Enemy");

	transform.translate = { 0.0f,10.0f,0.0f };
	transform.scale = { 10.0f,10.0f,10.0f };
	transform.rotate = Quaternion::identity;

	//collider_->SetCenter(transform.translate);
	////コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
	//collider_->SetSize(transform.scale * 2.0f);
	//collider_->SetOrientation(transform.rotate);
	//collider_->SetName("Enemy");
	//collider_->SetCallback([this](const CollisionInfo& collisionInfo) {OnCollision(collisionInfo); });

	bullets_.clear();
	barrierBullets_.clear();
	bigBullets_.clear();

	ResetCores();

	kMaxHp_ = CalcAllHp();

	hp_ = CalcAllHp();

	if (kMaxHp_ > 0) {
		hpWidth_ = float(1280.0f / kMaxHp_);
	}
	else {
		hpWidth_ = 0;
	}

	isDead_ = false;

	/*for (uint32_t i = 0; i < 2; i++) {
		crossAttack_.models_[i]->SetIsActive(false);
		crossAttack_.colliders_[i]->SetIsActive(false);
		crossAttack_.transforms[i].translate = { 0.0f,-1000.0f,0.0f };
		crossAttack_.transforms[i].scale = { 1.0f,1.0f,1.0f };
		crossAttack_.transforms[i].rotate = Quaternion::identity;
	}*/

	attackInterval_ = 150;
	WA_01_.attackCount = 3;
	attackTimer_ = attackInterval_;
	isStartAttack_ = false;
	WA_01_.startAttackInterval = 90;
	WA_01_.startAttackTimer = 0;

	workShot_.shotInterval = 300;
	workShot_.shotCount = 10;
	workShot_.shotTimer = workShot_.shotInterval;

	hpSprite_->SetPosition({ 0.0f,688.0f });
	hpSprite_->SetScale({ hpWidth_ * hp_, 64.0f });
	hpSprite_->SetAnchor({ 0.0f,0.5f });
	hpSprite_->SetDrawOrder(19);
	hpOverSprite_->SetTexcoordRect({ 0.0f,0.0f }, { 1280.0f,64.0f });
	hpOverSprite_->SetPosition({ 640.0f,688.0f });
	hpOverSprite_->SetScale({ 1280.0f, 64.0f });
	hpOverSprite_->SetDrawOrder(20);

	bullets_.clear();
	bigBullets_.clear();

	groundAttackSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/sound/boss_groundAttack.wav");
	shotSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/sound/boss_shot.wav");
	deathSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/sound/disolve.wav");

}

void Enemy::ResetCores() {

	EnemyCoreManager::GetInstance()->Clear();

	Transform tmpTransform = transform;
	tmpTransform.scale = { 5.0f,5.0f,5.0f };
	tmpTransform.translate = transform.translate + Vector3{ 0.0f,20.0f,0.0f };
	enemyCore_->Initialize(tmpTransform, 0);
	enemyCore_->SetPlayer(player_);
	EnemyCoreManager::GetInstance()->AddCore(enemyCore_);
	
}

void Enemy::Update() {

	bullets_.remove_if([](auto& bullet) {

		if (bullet->GetIsDead()) {
			BulletManager::GetInstance()->DeleteBullet(bullet->GetCollider()->GetGameObject());
			return true;
		}

		return false;

		});

	barrierBullets_.remove_if([](auto& bullet) {

		if (bullet->GetIsDead()) {
			BarrierBulletManager::GetInstance()->DeleteBullet(bullet->GetCollider()->GetGameObject());
			return true;
		}

		return false;

		});

	bigBullets_.remove_if([](auto& bullet) {

		if (bullet->GetIsDead()) {
			BulletManager::GetInstance()->DeleteBullet(bullet->GetCollider()->GetGameObject());
			return true;
		}

		return false;

		});

	if (!isDead_) {

		if (isStartAttack_) {
			Attack();
		}
		else {

			if (!enemyCore_->GetIsStan()) {

				//タイマー0で攻撃時の変数初期化、攻撃開始
				if (--attackTimer_ <= 0) {

					if (attackNumber_ == 1) {
						/*attackNumber_ = 1 + randomNumberGenerator.NextIntRange(0, 1);*/
						attackNumber_ = 1;
					}
					else {
						attackNumber_ = 1;
					}


					AttackInitialize();
					attackTimer_ = attackInterval_;
					isStartAttack_ = true;

				}

			}

		}

		enemyCore_->Update();

		for (auto& bullet : bullets_) {
			bullet->Update();
		}

		for (auto& bigBullet : bigBullets_) {
			bigBullet->Update();
		}

		for (auto& barrierBullet : barrierBullets_) {
			barrierBullet->Update();
		}

		hp_ = CalcAllHp();

		if (hp_ <= 0 && enemyCore_->GetIsDead()) {
			isDead_ = true;

			enemyCore_->SetIsActiveModel(false);

			Audio::GetInstance()->SoundPlayWave(deathSE_);
		}

		

	}


	hpSprite_->SetScale({ hpWidth_ * hp_, 64.0f });

	transform.UpdateMatrix();
	/*model_->SetWorldMatrix(transform.worldMatrix);

	collider_->SetCenter(transform.translate);
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetOrientation(transform.rotate);*/

	if (hitCoolTime_ > 0) {
		hitCoolTime_--;
	}

	//死んだとき
	//if (isDead_) {

	//	model_->SetIsActive(false);

	//}
	////生きている時
	//else {

	//	if (hitCoolTime_ % 2 == 0) {
	//		model_->SetIsActive(true);
	//	}
	//	else {
	//		model_->SetIsActive(false);
	//	}
	//	
	//}

}

void Enemy::Attack() {

	switch (attackNumber_)
	{
	//使用しない
	case 0:

		

		break;
	default:
	case 1:

		for (auto& bullet : bullets_) {

			if (workShot_.shotTimer % 20 == 0 && workShot_.shotTimer <= 240 && !bullet->GetIsShot()) {
				bullet->Shot(player_->GetPosition());
				Audio::GetInstance()->SoundPlayWave(shotSE_);
				break;
			}

		}

		for (auto& bullet : barrierBullets_) {

			if (workShot_.shotTimer % 20 == 0 && workShot_.shotTimer <= 120 && !bullet->GetIsShot()) {
				bullet->Shot(player_->GetPosition());
				Audio::GetInstance()->SoundPlayWave(shotSE_);
				break;
			}

		}

		if (--workShot_.shotTimer <= 0) {
			isStartAttack_ = false;
		}

		break;
	case 2:

		

		break;
	}

}

void Enemy::AttackInitialize() {

	switch (attackNumber_)
	{
	case 0:

		

		break;
	default:
	case 1:

		AddBullet();
		workShot_.shotTimer = workShot_.shotInterval;

		break;
	case 2:

		
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
	else if (collisionInfo.collider->GetName() == "Block_Shot") {

		if (hitCoolTime_ == 0) {
			hp_ -= 3;
			hitCoolTime_ = invincibleTime_;
		}

	}

}

void Enemy::AddBullet() {

	for (uint32_t i = 0; i < workShot_.shotCount; i++) {

		
		//最大数十個まで
		if (i < 5 && barrierBullets_.size() < 10) {
			std::shared_ptr<BarrierBullet> newBullet = std::make_shared<BarrierBullet>();
			newBullet->Initialize(transform.translate + Vector3{ (i - 5.0f) * 5.0f,
				randomNumberGenerator.NextFloatRange(-5.0f,5.0f), randomNumberGenerator.NextFloatRange(-5.0f,5.0f) });
			BarrierBulletManager::GetInstance()->AddBullet(newBullet);
			barrierBullets_.push_back(newBullet);
		}
		else {
			std::shared_ptr<EnemyBullet> newBullet = std::make_shared<EnemyBullet>();
			newBullet->Initialize(transform.translate + Vector3{ (i - 5.0f) * 5.0f,
				randomNumberGenerator.NextFloatRange(-5.0f,5.0f), randomNumberGenerator.NextFloatRange(-5.0f,5.0f) });
			BulletManager::GetInstance()->AddBullet(newBullet);
			bullets_.push_back(newBullet);
		}

		

	}

}

int32_t Enemy::CalcAllHp() {

	int32_t totalHp = 0;

	totalHp = enemyCore_->GetHp();

	return totalHp;

}

