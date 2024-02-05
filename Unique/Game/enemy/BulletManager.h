#pragma once
#include "EnemyBullet.h"

class BulletManager
{
public:

	static BulletManager* GetInstance();

	void AddBullet(std::shared_ptr<EnemyBullet> bullet) {
		bulletList_[bullet.get()] = bullet;
	}

	std::shared_ptr<EnemyBullet> GetBullet(GameObject* object) {
		return bulletList_[object];
	}

	void DeleteBullet(GameObject* object) { bulletList_.erase(object); }

	void Clear() { bulletList_.clear(); }

private:

	BulletManager() = default;
	~BulletManager() = default;
	BulletManager(const BulletManager&) = delete;
	BulletManager& operator=(const BulletManager&) = delete;

	std::unordered_map<GameObject*, std::shared_ptr<EnemyBullet>> bulletList_;

};



