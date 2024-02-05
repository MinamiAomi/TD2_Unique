#pragma once
#include "BarrierBullet.h"

class BarrierBulletManager
{
public:

	static BarrierBulletManager* GetInstance();

	void AddBullet(std::shared_ptr<BarrierBullet> bullet) {
		bulletList_[bullet.get()] = bullet;
	}

	std::shared_ptr<BarrierBullet> GetBullet(GameObject* object) {
		return bulletList_[object];
	}

	void DeleteBullet(GameObject* object) { bulletList_.erase(object); }

	void Clear() { bulletList_.clear(); }

private:

	BarrierBulletManager() = default;
	~BarrierBulletManager() = default;
	BarrierBulletManager(const BarrierBulletManager&) = delete;
	BarrierBulletManager& operator=(const BarrierBulletManager&) = delete;

	std::unordered_map<GameObject*, std::shared_ptr<BarrierBullet>> bulletList_;

};



