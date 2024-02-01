#include "PlayerModel.h"

#include "Graphics/ResourceManager.h"

const std::array<std::string, PlayerModel::kNumParts> PlayerModel::kPartNames = {
    "Hip",
    "Body",
    "Head",
    "LeftShoulder",
    "LeftUpperArm",
    "LeftLowerArm",
    "RightShoulder",
    "RightUpperArm",
    "RightLowerArm",
    "LeftPelvis",
    "LeftUpperLeg",
    "LeftLowerLeg",
    "RightPelvis",
    "RightUpperLeg",
    "RightLowerLeg",
};

const std::array<std::string, PlayerModel::kNumAnimationTypes> PlayerModel::kAnimationTypeNames = {
    "Player_Wait",
    "Player_Attack1",
    "Player_Attack2",
    "Player_Attack3",
};

const std::array<PlayerModel::AnimationPrameter, PlayerModel::kNumAnimationTypes> PlayerModel::kAnimationParameters = {
    {   { 60,  true  },
        { 60,  true },
        { 60,  true },
        { 150, true }   },
};

void PlayerModel::Initialize(const Transform* baseTransform) {
    auto resources = ResourceManager::GetInstance();
    baseTransform_ = baseTransform;

    // モデルとトランスフォームを初期化
    for (uint32_t i = 0; i < kNumParts; ++i) {
        models_[i] = std::make_unique<ModelInstance>();
        // 腰モデルは消す
        if (ModelActivePart(static_cast<Part>(i))) {
            models_[i]->SetModel(resources->FindModel(kPartNames[i]));
        }
        else {
            models_[i]->SetIsActive(false);
        }

        transforms_[i] = std::make_unique<Transform>();
        transforms_[i]->translate = Vector3::zero;
        transforms_[i]->scale = Vector3::one;
        transforms_[i]->rotate = Quaternion::identity;
    }

    // transformが基礎
    transforms_[kHip]->SetParent(baseTransform_);
    //腰から体に親子付け
    transforms_[kBody]->SetParent(transforms_[kHip].get());
    //体から頭に親子付け
    transforms_[kHead]->SetParent(transforms_[kBody].get());
    //体から両腕に順番に親子付け
    transforms_[kLeftShoulder]->SetParent(transforms_[kBody].get());
    transforms_[kLeftUpperArm]->SetParent(transforms_[kLeftShoulder].get());
    transforms_[kLeftLowerArm]->SetParent(transforms_[kLeftUpperArm].get());
    transforms_[kRightShoulder]->SetParent(transforms_[kBody].get());
    transforms_[kRightUpperArm]->SetParent(transforms_[kRightShoulder].get());
    transforms_[kRightLowerArm]->SetParent(transforms_[kRightUpperArm].get());
    //腰から両足に順番に親子付け
    transforms_[kLeftPelvis]->SetParent(baseTransform_);
    transforms_[kLeftUpperLeg]->SetParent(transforms_[kLeftPelvis].get());
    transforms_[kLeftLowerLeg]->SetParent(transforms_[kLeftUpperLeg].get());
    transforms_[kRightPelvis]->SetParent(baseTransform_);
    transforms_[kRightUpperLeg]->SetParent(transforms_[kRightPelvis].get());
    transforms_[kRightLowerLeg]->SetParent(transforms_[kRightUpperLeg].get());

    // アニメーションリソースを取得
    for (uint32_t i = 0; i < kNumAnimationTypes; ++i) {
        animations_[i] = resources->FindHierarchicalAnimation(kAnimationTypeNames[i]);
    }

    currentAnimationType_ = kAttack1;
    animationParameter_ = 0.0f;
    playSpeedScale_ = 1.0f;
    isStopping_ = false;
}

void PlayerModel::Update() {
    if (!isStopping_) {
        float delta = 1.0f / kAnimationParameters[currentAnimationType_].frameDuration;
        UpdateAnimationParameter(delta, kAnimationParameters[currentAnimationType_].isLoop);
    }

    for (uint32_t i = 0; i < kNumParts; ++i) {
        auto& animationNode = animations_[currentAnimationType_]->GetNode(kPartNames[i]);
        transforms_[i]->translate = animationNode.translate.GetInterpolatedValue(animationParameter_);
        transforms_[i]->rotate = animationNode.rotate.GetInterpolatedValue(animationParameter_);
        transforms_[i]->scale = animationNode.scale.GetInterpolatedValue(animationParameter_);
        transforms_[i]->UpdateMatrix();
        Matrix4x4 mat = animationNode.initialInverseMatrix * transforms_[i]->worldMatrix;

        if (ModelActivePart(static_cast<Part>(i))) {
            models_[i]->SetWorldMatrix(mat);
        }
    }
}

void PlayerModel::PlayAnimation(AnimationType animation, float playSpeedScale) {
    currentAnimationType_ = animation;
    animationParameter_ = 0.0f;
    playSpeedScale_ = playSpeedScale;
    isStopping_ = false;
}

void PlayerModel::StopAnimation() {
    isStopping_ = true;
}

void PlayerModel::RestartAnimation() {
    isStopping_ = false;
}

void PlayerModel::SetIsActive(bool isActive) {
    for (auto& model : models_) {
        model->SetIsActive(isActive);
    }
}

bool PlayerModel::ModelActivePart(Part part) {
    return !(part == kLeftPelvis || part == kRightPelvis);
}

bool PlayerModel::UpdateAnimationParameter(float delta, bool isLoop) {
    animationParameter_ += delta * playSpeedScale_;
    if (animationParameter_ >= 1.0f) {
        if (!isLoop) {
            animationParameter_ = 1.0f;
            currentAnimationType_ = kWait;
            return false;
        }
        animationParameter_ -= static_cast<float>(static_cast<int>(animationParameter_));
    }
    return true;
}

//for (int i = 0; i < kMaxParts; ++i) {
//    playerTransforms_[i]->translate = waitAnimation_->GetNode(kPlayerPartsStr[i]).translate.GetInterpolatedValue(0);
//    playerTransforms_[i]->rotate = waitAnimation_->GetNode(kPlayerPartsStr[i]).rotate.GetInterpolatedValue(0);
//    playerTransforms_[i]->scale = waitAnimation_->GetNode(kPlayerPartsStr[i]).scale.GetInterpolatedValue(0);
//    playerTransforms_[i]->UpdateMatrix();
//    playerModels_[i]->SetWorldMatrix(waitAnimation_->GetNode(kPlayerPartsStr[i]).initialInverseMatrix * playerTransforms_[i]->worldMatrix * Matrix4x4::MakeScaling(Vector3(2)));
//}