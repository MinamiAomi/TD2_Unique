#pragma once

#include <array>
#include <optional>

#include "Math/Transform.h"
#include "Graphics/Model.h"
#include "Graphics/HierarchicalAnimation.h"

// プレイヤーのモデルを再生するクラス
class PlayerModel {
public:
    //プレイヤーパーツのナンバリング
    enum Part {
        kHip, //腰
        kBody, //体
        kHead, //頭
        kLeftShoulder, //左肩
        kLeftUpperArm, //左腕上部
        kLeftLowerArm, //左腕下部
        kRightShoulder, //右肩
        kRightUpperArm, //右腕上部
        kRightLowerArm, //右腕下部
        kLeftPelvis, // 左足付け根(モデル無し)
        kLeftUpperLeg, //左足上部
        kLeftLowerLeg, //左足下部
        kRightPelvis, // 右足付け根(モデル無し)
        kRightUpperLeg, //右足上部
        kRightLowerLeg, //右足下部

        kNumParts, //最大パーツ数。配列の数を設定する時等に使用
    };

    enum AnimationType {
        kWait,
        kAttack1,
        kAttack2,
        kAttack3,

        kNumAnimationTypes
    };

    void Initialize(const Transform* baseTransform);
    void Update(AnimationType animation, float parameter);

    const Transform& GetTransform(Part part) const { return *transforms_[part]; }

    void SetIsActive(bool isActive);

private:
    static const std::array<std::string, kNumParts> kPartNames;
    static const std::array<std::string, kNumAnimationTypes> kAnimationTypeNames;
    
    bool ModelActivePart(Part part);

    const Transform* baseTransform_;
    std::array<std::unique_ptr<ModelInstance>, kNumParts> models_;
    std::array<std::unique_ptr<Transform>, kNumParts> transforms_;
    std::array<std::shared_ptr<HierarchicalAnimation>, kNumAnimationTypes> animations_;
};