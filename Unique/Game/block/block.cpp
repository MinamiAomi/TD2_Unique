#include "block.h"
#include "Graphics/ResourceManager.h"

Block::Block()
{
	model_ = std::make_shared<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("Cube"));
}

Block::~Block()
{
}

void Block::Initialize(const Vector3& pos, Player* player, const Vector3& blockSize) {

	SetName("Block");

	player_ = player;

	blockSize_ = blockSize;

	sizeX_ = uint32_t(fabsf(blockSize.x));
	sizeY_ = uint32_t(fabsf(blockSize.y));
	sizeZ_ = uint32_t(fabsf(blockSize.z));

	particleCount_ = sizeX_ * sizeY_ * sizeZ_;

	transform.translate = pos;
	transform.scale = blockSize_;
	transform.rotate = player->transform.rotate;

	collider_ = std::make_unique<BoxCollider>();
	collider_->SetCenter(transform.translate);
	//コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetOrientation(transform.rotate);
	collider_->SetName("Block_Stay");
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) {OnCollision(collisionInfo); });

}

void Block::Update() {

	/*Vector3 posA = { model_->position_.x, 0.0f,model_->position_.z };
	Vector3 posB = { player_->GetLocalPosition().x, 0.0f,player_->GetLocalPosition().z };

	if (player_->GetIsBreak() && Length(posB - posA) < 20.0f) {
		isDead_ = true;
	}*/

	if (!isDead_) {

		if (!isShot_) {

			if (--liveTimer_ <= 0) {
				isDead_ = true;
			}

		}
		else {

			transform.translate += velocity_;
			transform.rotate = Quaternion::MakeForYAxis(0.1f) * transform.rotate;

			if (--shotLiveTimer_ <= 0) {
				isDead_ = true;
			}

		}

	}

	transform.UpdateMatrix();

	collider_->SetCenter(transform.translate);
	//コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetOrientation(transform.rotate);

	model_->SetWorldMatrix(transform.worldMatrix);

}

void Block::Shot(const Vector3& velocity) {

	velocity_ = velocity;
	velocity_.Normalize();
	velocity_ = player_->transform.rotate * velocity_;
	velocity_ *= 3.0f;
	isShot_ = true;
	collider_->SetName("Block_Shot");
	collider_->SetIsActive(true);

}

void Block::OnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Enemy" ||
		collisionInfo.collider->GetName() == "Enemy_Core") {

		isDead_ = true;
		collider_->SetIsActive(false);
		
	}
	else if (collisionInfo.collider->GetName() == "Enemy_Bullet") {

		canShot_ = true;
		model_->SetColor({ 1.0f,1.0f,0.0f });

	}
	else if (collisionInfo.collider->GetName() == "Weapon") {

		if (canShot_ && !isShot_) {
			Shot({ 0.0f,0.2f,1.0f });
		}

	}

}
