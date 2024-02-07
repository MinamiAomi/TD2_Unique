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
    "Player_DushStart",
    "Player_DushEnd",
    "Player_GuardStart",
    "Player_GuardEnd",
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
}

void PlayerModel::Update(AnimationType animation, float parameter) {

    for (uint32_t i = 0; i < kNumParts; ++i) {
        auto& animationNode = animations_[animation]->GetNode(kPartNames[i]);
        transforms_[i]->translate = animationNode.translate.GetInterpolatedValue(parameter);
        transforms_[i]->rotate = animationNode.rotate.GetInterpolatedValue(parameter);
        transforms_[i]->scale = animationNode.scale.GetInterpolatedValue(parameter);
        transforms_[i]->UpdateMatrix();
        Matrix4x4 mat = animationNode.initialInverseMatrix * transforms_[i]->worldMatrix;

        if (ModelActivePart(static_cast<Part>(i))) {
            models_[i]->SetWorldMatrix(mat);
        }
    }
}

void PlayerModel::SetIsActive(bool isActive) {
    for (auto& model : models_) {
        model->SetIsActive(isActive);
    }
}

bool PlayerModel::ModelActivePart(Part part) {
    return !(part == kLeftPelvis || part == kRightPelvis);
}

//for (int i = 0; i < kMaxParts; ++i) {
//    playerTransforms_[i]->translate = waitAnimation_->GetNode(kPlayerPartsStr[i]).translate.GetInterpolatedValue(0);
//    playerTransforms_[i]->rotate = waitAnimation_->GetNode(kPlayerPartsStr[i]).rotate.GetInterpolatedValue(0);
//    playerTransforms_[i]->scale = waitAnimation_->GetNode(kPlayerPartsStr[i]).scale.GetInterpolatedValue(0);
//    playerTransforms_[i]->UpdateMatrix();
//    playerModels_[i]->SetWorldMatrix(waitAnimation_->GetNode(kPlayerPartsStr[i]).initialInverseMatrix * playerTransforms_[i]->worldMatrix * Matrix4x4::MakeScaling(Vector3(2)));
//}