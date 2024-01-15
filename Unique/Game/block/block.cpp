#include "block.h"
#include "Graphics/ResourceManager.h"

Block::Block()
{
}

Block::~Block()
{
	model_ = std::make_shared<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("Cube"));
}

void Block::Initialize(const Vector3& pos, Player* player, const Vector3& blockSize) {

	SetName("Block");

	blockSize_ = blockSize;

	sizeX_ = uint32_t(fabsf(blockSize.x));
	sizeY_ = uint32_t(fabsf(blockSize.y));
	sizeZ_ = uint32_t(fabsf(blockSize.z));

	particleCount_ = sizeX_ * sizeY_ * sizeZ_;

	transform.translate = pos;
	transform.scale = blockSize_;

	collider_ = std::make_unique<BoxCollider>();
	collider_->SetCenter(transform.translate);
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetName("Block");
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) {OnCollision(collisionInfo); });

	player_ = player;

}

void Block::Update() {

	/*Vector3 posA = { model_->position_.x, 0.0f,model_->position_.z };
	Vector3 posB = { player_->GetLocalPosition().x, 0.0f,player_->GetLocalPosition().z };

	if (player_->GetIsBreak() && Length(posB - posA) < 20.0f) {
		isDead_ = true;
	}*/

	if (!isShot_) {

		if (--liveTimer_ <= 0) {
			isDead_ = true;
		}

	}
	else {

		transform.translate += velocity_;

		if (--shotLiveTimer_ <= 0) {
			isDead_ = true;
		}

	}

	transform.UpdateMatrix();

	collider_->SetCenter(transform.translate);
	
	model_->SetWorldMatrix(transform.worldMatrix);

}

void Block::Shot(const Vector3& velocity) {

	velocity_ = velocity;
	velocity_ *= 3.0f;
	isShot_ = true;

}

void Block::OnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Enemy") {

		isDead_ = true;
		
	}

}
