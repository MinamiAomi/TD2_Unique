#include "Animation.h"

#include <cassert>

size_t AnimationNode::GetPositionIndex(float animationTime) {
    size_t min = 0, max = positions_.size() - 1, mid = 0;

    do {
        mid = (min + max) >> 1;
        if (animationTime >= positions_[mid].timeStamp && 
            animationTime <= positions_[mid + 1].timeStamp) {
            return mid;
        }
        else if (animationTime < positions_[mid].timeStamp) {
            max = mid - 1;
        }
        else {
            min = mid + 1;
        }

    } while (min <= max);

    return positions_.size();
}

size_t AnimationNode::GetRotateIndex(float animationTime) {
    size_t min = 0, max = rotates_.size() - 1, mid = 0;

    do {
        mid = (min + max) >> 1;
        if (animationTime >= rotates_[mid].timeStamp &&
            animationTime <= rotates_[mid + 1].timeStamp) {
            return mid;
        }
        else if (animationTime < rotates_[mid].timeStamp) {
            max = mid - 1;
        }
        else {
            min = mid + 1;
        }

    } while (min <= max);

    return rotates_.size();
}

size_t AnimationNode::GetScaleIndex(float animationTime) {
    size_t min = 0, max = scales_.size() - 1, mid = 0;

    do {
        mid = (min + max) >> 1;
        if (animationTime >= scales_[mid].timeStamp &&
            animationTime <= scales_[mid + 1].timeStamp) {
            return mid;
        }
        else if (animationTime < scales_[mid].timeStamp) {
            max = mid - 1;
        }
        else {
            min = mid + 1;
        }

    } while (min <= max);

    return scales_.size();
}
