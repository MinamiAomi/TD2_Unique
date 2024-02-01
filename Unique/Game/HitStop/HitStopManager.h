#pragma once
#include <cmath>

class HitStopManager
{
public:

	static HitStopManager* GetInstance();

	/// <summary>
	/// valフレーム分ヒットストップ
	/// </summary>
	/// <param name="val"></param>
	void StopFrame(int32_t val) { hitStopCount_ = val; }

	/// <summary>
	/// カウントをval分減らす
	/// </summary>
	/// <param name="val"></param>
	void SubHitStopFrame(int32_t val = 1) { hitStopCount_ -= val; }

	/// <summary>
	/// 現在のヒットストップフレームを返す
	/// </summary>
	/// <returns></returns>
	int32_t GetCount()const { return hitStopCount_; }

private:

	HitStopManager() = default;
	~HitStopManager() = default;
	HitStopManager(const HitStopManager&) = delete;
	HitStopManager& operator=(const HitStopManager&) = delete;

	int32_t hitStopCount_ = 0;

};
