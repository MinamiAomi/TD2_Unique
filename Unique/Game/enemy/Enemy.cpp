#include "Enemy.h"
#include "Math/Random.h"
#include "Graphics/ResourceManager.h"
#include "EnemyCoreManager.h"

static Random::RandomNumberGenerator randomNumberGenerator;

Enemy::Enemy()
{
	/*model_ = std::make_shared<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("Cube"));*/

	for (uint32_t i = 0; i < 10; i++) {
		attackModels_[i] = std::make_shared<ModelInstance>();
		attackModels_[i]->SetModel(ResourceManager::GetInstance()->FindModel("Cube"));
		attackModels_[i]->SetIsActive(false);
	}

	for (uint32_t i = 0; i < 8; i++) {
		enemyCores_[i] = std::make_shared<EnemyCore>();
	}

	hpTex_ = ResourceManager::GetInstance()->FindTexture("enemy_hp");

	hpSprite_ = std::make_unique<Sprite>();
	hpSprite_->SetTexture(hpTex_);

	/*collider_ = std::make_unique<BoxCollider>();*/

	/*crossAttack_.models_[0] = std::make_shared<ModelInstance>();
	crossAttack_.models_[0]->SetModel(ResourceManager::GetInstance()->FindModel("Cube"));
	crossAttack_.models_[0]->SetIsActive(false);
	crossAttack_.models_[1] = std::make_shared<ModelInstance>();
	crossAttack_.models_[1]->SetModel(ResourceManager::GetInstance()->FindModel("Cube"));
	crossAttack_.models_[1]->SetIsActive(false);

	crossAttack_.colliders_[0] = std::make_unique<BoxCollider>();
	crossAttack_.colliders_[0]->SetName("Enemy_Bullet");
	crossAttack_.colliders_[0]->SetIsActive(false);
	crossAttack_.colliders_[1] = std::make_unique<BoxCollider>();
	crossAttack_.colliders_[1]->SetName("Enemy_Bullet");
	crossAttack_.colliders_[1]->SetIsActive(false);*/

	

}

Enemy::~Enemy()
{
}

void Enemy::Initialize() {

	SetName("Enemy");

	transform.translate = { 0.0f,5.0f,50.0f };
	transform.scale = { 10.0f,10.0f,10.0f };
	transform.rotate = Quaternion::identity;

	//collider_->SetCenter(transform.translate);
	////コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
	//collider_->SetSize(transform.scale * 2.0f);
	//collider_->SetOrientation(transform.rotate);
	//collider_->SetName("Enemy");
	//collider_->SetCallback([this](const CollisionInfo& collisionInfo) {OnCollision(collisionInfo); });

	ResetCores();

	kMaxHp_ = CalcAllHp();

	hp_ = CalcAllHp();

	if (kMaxHp_ > 0) {
		hpWidth_ = float(300.0f / kMaxHp_);
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
	workAttack_01_.attackCount = 3;
	attackTimer_ = attackInterval_;
	isStartAttack_ = false;
	workAttack_01_.startAttackInterval = 90;
	workAttack_01_.startAttackTimer = 0;

	workShot_.shotInterval = 180;
	workShot_.shotCount = 10;
	workShot_.shotTimer = workShot_.shotInterval;

	hpSprite_->SetPosition({ 640.0f,600.0f });
	hpSprite_->SetScale({ hpWidth_ * hp_, 64.0f });

	bullets_.clear();
	bigBullets_.clear();

	groundAttackSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/proto_sound/boss_groundAttack.wav");
	shotSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/proto_sound/boss_shot.wav");
	deathSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/proto_sound/disolve.wav");

}

void Enemy::ResetCores() {

	EnemyCoreManager::GetInstance()->Clear();

	Transform tmpTransform = transform;
	tmpTransform.scale = { 5.0f,5.0f,5.0f };
	//左下前
	tmpTransform.translate = transform.translate + Vector3{ -7.5f,0.0f,-7.5f };
	enemyCores_[kLeftDownFront]->Initialize(tmpTransform, 0);
	EnemyCoreManager::GetInstance()->AddCore(enemyCores_[kLeftDownFront]);
	//左下奥
	tmpTransform.translate = transform.translate + Vector3{ -7.5f,0.0f,7.5f };
	enemyCores_[kLeftDownBack]->Initialize(tmpTransform, 1);
	EnemyCoreManager::GetInstance()->AddCore(enemyCores_[kLeftDownBack]);
	//左上前
	tmpTransform.translate = transform.translate + Vector3{ -7.5f,12.5f,-7.5f };
	enemyCores_[kLeftTopFront]->Initialize(tmpTransform, 2);
	EnemyCoreManager::GetInstance()->AddCore(enemyCores_[kLeftTopFront]);
	//左上奥
	tmpTransform.translate = transform.translate + Vector3{ -7.5f,12.5f,7.5f };
	enemyCores_[kLeftTopBack]->Initialize(tmpTransform, 3);
	EnemyCoreManager::GetInstance()->AddCore(enemyCores_[kLeftTopBack]);
	//右下前
	tmpTransform.translate = transform.translate + Vector3{ 7.5f,0.0f,-7.5f };
	enemyCores_[kRightDownFront]->Initialize(tmpTransform, 4);
	EnemyCoreManager::GetInstance()->AddCore(enemyCores_[kRightDownFront]);
	//右下奥
	tmpTransform.translate = transform.translate + Vector3{ 7.5f,0.0f,7.5f };
	enemyCores_[kRightDownBack]->Initialize(tmpTransform, 5);
	EnemyCoreManager::GetInstance()->AddCore(enemyCores_[kRightDownBack]);
	//右上前
	tmpTransform.translate = transform.translate + Vector3{ 7.5f,12.5f,-7.5f };
	enemyCores_[kRightTopFront]->Initialize(tmpTransform, 6);
	EnemyCoreManager::GetInstance()->AddCore(enemyCores_[kRightTopFront]);
	//右上奥
	tmpTransform.translate = transform.translate + Vector3{ 7.5f,12.5f,7.5f };
	enemyCores_[kRightTopBack]->Initialize(tmpTransform, 7);
	EnemyCoreManager::GetInstance()->AddCore(enemyCores_[kRightTopBack]);
}

void Enemy::Update() {

	bullets_.remove_if([](auto& bullet) {

		if (bullet->GetIsDead()) {
			return true;
		}

		return false;

		});

	bigBullets_.remove_if([](auto& bullet) {

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
				
				if (attackNumber_ == 1) {
					/*attackNumber_ = 1 + randomNumberGenerator.NextIntRange(0, 1);*/
					attackNumber_ = 2;
				}
				else {
					attackNumber_ = 1;
				}
				

				AttackInitialize();
				attackTimer_ = attackInterval_;
				isStartAttack_ = true;

			}

		}

		for (auto& core : enemyCores_) {
			core->Update();
		}

		for (auto& bullet : bullets_) {
			bullet->Update();
		}

		for (auto& bigBullet : bigBullets_) {
			bigBullet->Update();
		}

		hp_ = CalcAllHp();

		if (hp_ <= 0) {
			isDead_ = true;

			for (auto& core : enemyCores_) {
				core->SetIsActiveModel(false);
			}

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

	//地面攻撃中
	if (isStartAttack_ && attackNumber_ == 0) {

		for (uint32_t i = 0; i < workAttack_01_.attackCount; i++) {
			attackModels_[i]->SetIsActive(true);
		}

	}
	else {

		for (uint32_t i = 0; i < workAttack_01_.attackCount; i++) {
			attackModels_[i]->SetIsActive(false);
		}

	}

}

void Enemy::Attack() {

	switch (attackNumber_)
	{
	//使用しない
	case 0:

		//カウントが30を切ったら攻撃開始
		if (--workAttack_01_.startAttackTimer < 30) {

			for (uint32_t i = 0; i < workAttack_01_.attackCount; i++) {

				attackTransforms_[i].translate.y += attackSizes_[i].y * 2.0f / 30.0f;
				attackTransforms_[i].UpdateMatrix();
				attackColliders_[i]->SetCenter(attackTransforms_[i].translate);
				attackModels_[i]->SetWorldMatrix(attackTransforms_[i].worldMatrix);

				/*if (OnCollision(attackColliders_[i], player_->GetCollision())) {
					player_->Damage(3);
				}*/

			}

			if (workAttack_01_.startAttackTimer <= 0) {

				/*if (blocksPtr_ && isStartAttack_) {

					for (uint32_t i = 0; i < workAttack_.attackCount; i++) {
						std::shared_ptr<Block> block = std::make_shared<Block>();
						block->Initialize(attackTransforms_[i].translate,
							player_, attackSizes_[i]);
						blocksPtr_->push_back(block);
					}

				}*/

				isStartAttack_ = false;

			}

		}
		else {

			if (workAttack_01_.startAttackTimer == 30) {
				Audio::GetInstance()->SoundPlayWave(groundAttackSE_);
			}

		}

		break;
	default:
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
	case 2:

		//移動が完了したら攻撃行動開始
		if (enemyCores_[kLeftTopFront]->lerpT_ > 0.99f &&
			enemyCores_[kRightTopFront]->lerpT_ > 0.99f) {

			for (auto& bigBullet : bigBullets_) {

				bigBullet->SetIsActive(true);

				if (crossAttack_.attackTimer >= 90) {

					if (crossAttack_.attackTimer == 90) {
						bigBullet->Shot({ 0.0f,0.0f,-20.0f });
						Audio::GetInstance()->SoundPlayWave(shotSE_);
					}
					else {
						bigBullet->transform.scale += {0.05f, 0.05f, 0.05f};
					}

				}

			}

			/*for (uint32_t i = 0; i < 2; i++) {
				crossAttack_.transforms[i].UpdateMatrix();
				crossAttack_.models_[i]->SetWorldMatrix(crossAttack_.transforms[i].worldMatrix);
				crossAttack_.colliders_[i]->SetCenter(crossAttack_.transforms[i].translate);
				crossAttack_.colliders_[i]->SetSize(crossAttack_.transforms[i].scale);
				crossAttack_.colliders_[i]->SetOrientation(crossAttack_.transforms[i].rotate);
			}*/

			if (--crossAttack_.attackTimer <= 0) {
				isStartAttack_ = false;
				enemyCores_[kLeftTopFront]->startPosition_ = crossAttack_.shotPosition[0];
				enemyCores_[kLeftTopFront]->endPosition_ = transform.translate + Vector3{ -7.5f,12.5f,-7.5f };
				enemyCores_[kLeftTopFront]->lerpT_ = 0.0f;
				enemyCores_[kRightTopFront]->startPosition_ = crossAttack_.shotPosition[1];
				enemyCores_[kRightTopFront]->endPosition_ = transform.translate + Vector3{ 7.5f,12.5f,-7.5f };
				enemyCores_[kRightTopFront]->lerpT_ = 0.0f;
				/*SetCoresToRoot();*/
			}

		}

		break;
	}

}

void Enemy::AttackInitialize() {

	switch (attackNumber_)
	{
	case 0:

		workAttack_01_.attackCount = randomNumberGenerator.NextIntRange(3, 6);

		for (uint32_t i = 0; i < workAttack_01_.attackCount; i++) {
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
		workAttack_01_.startAttackTimer = workAttack_01_.startAttackInterval;

		break;
	default:
	case 1:

		AddBullet();
		workShot_.shotTimer = workShot_.shotInterval;

		break;
	case 2:

		crossAttack_.attackTimer = crossAttack_.maxAttackTime;
		crossAttack_.shotPosition[0] = { -30.0f,0.0f,40.0f };
		crossAttack_.shotPosition[1] = { 30.0f,0.0f,40.0f };

		for (uint32_t i = 0; i < 2; i++) {
			std::shared_ptr<EnemyBullet> newBullet = std::make_shared<EnemyBullet>();

			newBullet->Initialize(crossAttack_.shotPosition[i]);
			newBullet->SetIsActive(false);

			bigBullets_.push_back(newBullet);

		}

		/*crossAttack_.transforms[0].translate = crossAttack_.shotPosition[0];
		crossAttack_.transforms[1].translate = crossAttack_.shotPosition[1];*/
		enemyCores_[kLeftTopFront]->startPosition_ = transform.translate + Vector3{ -7.5f,12.5f,-7.5f };
		enemyCores_[kLeftTopFront]->endPosition_ = crossAttack_.shotPosition[0];
		enemyCores_[kLeftTopFront]->lerpT_ = 0.0f;
		enemyCores_[kRightTopFront]->startPosition_ = transform.translate + Vector3{ 7.5f,12.5f,-7.5f };
		enemyCores_[kRightTopFront]->endPosition_ = crossAttack_.shotPosition[1];
		enemyCores_[kRightTopFront]->lerpT_ = 0.0f;

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

int32_t Enemy::CalcAllHp() {

	int32_t totalHp = 0;

	for (auto& core : enemyCores_) {

		totalHp += core->GetHp();

	}

	return totalHp;

}

void Enemy::SetCoresToRoot() {

	Transform tmpTransform = transform;
	//左下前
	tmpTransform.translate = transform.translate + Vector3{ -7.5f,0.0f,-7.5f };
	enemyCores_[kLeftDownFront]->transform.translate = tmpTransform.translate;
	//左下奥
	tmpTransform.translate = transform.translate + Vector3{ -7.5f,0.0f,7.5f };
	enemyCores_[kLeftDownBack]->transform.translate = tmpTransform.translate;
	//左上前
	tmpTransform.translate = transform.translate + Vector3{ -7.5f,12.5f,-7.5f };
	enemyCores_[kLeftTopFront]->transform.translate = tmpTransform.translate;
	//左上奥
	tmpTransform.translate = transform.translate + Vector3{ -7.5f,12.5f,7.5f };
	enemyCores_[kLeftTopBack]->transform.translate = tmpTransform.translate;
	//右下前
	tmpTransform.translate = transform.translate + Vector3{ 7.5f,0.0f,-7.5f };
	enemyCores_[kRightDownFront]->transform.translate = tmpTransform.translate;
	//右下奥
	tmpTransform.translate = transform.translate + Vector3{ 7.5f,0.0f,7.5f };
	enemyCores_[kRightDownBack]->transform.translate = tmpTransform.translate;
	//右上前
	tmpTransform.translate = transform.translate + Vector3{ 7.5f,12.5f,-7.5f };
	enemyCores_[kRightTopFront]->transform.translate = tmpTransform.translate;
	//右上奥
	tmpTransform.translate = transform.translate + Vector3{ 7.5f,12.5f,7.5f };
	enemyCores_[kRightTopBack]->transform.translate = tmpTransform.translate;

}
