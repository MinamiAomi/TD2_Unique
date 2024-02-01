#include "Effect.h"
#include "Graphics/ResourceManager.h"

Effect::Effect()
{
	model_->SetModel(ResourceManager::GetInstance()->FindModel("Plane"));
}

Effect::~Effect()
{
}

void Effect::Initialize() {

}

void Effect::Update() {

	//ビルボード設定
	if (camera_) {
		billBoardMatrix_ = camera_->GetViewMatrix().Inverse();
		billBoardMatrix_.m[3][0] = 0.0f;
		billBoardMatrix_.m[3][1] = 0.0f;
		billBoardMatrix_.m[3][2] = 0.0f;
	}


}


