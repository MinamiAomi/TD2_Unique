#pragma once
#include "SmallEnemy.h"

class SmallEnemyManager
{
public:

	static SmallEnemyManager* GetInstance();

	void AddEnemy(const std::shared_ptr<SmallEnemy>& core) {
		enemyList_[core.get()] = core;
	}

	std::shared_ptr<SmallEnemy> GetEnemy(GameObject* object) { 
		return enemyList_[object]; 
	}

	void DeleteEnemy(GameObject* object) { enemyList_.erase(object); }

	void Clear() { enemyList_.clear(); }

private:

	SmallEnemyManager() = default;
	~SmallEnemyManager() = default;
	SmallEnemyManager(const SmallEnemyManager&) = delete;
	SmallEnemyManager& operator=(const SmallEnemyManager&) = delete;

	std::unordered_map<GameObject*, std::shared_ptr<SmallEnemy>> enemyList_;

};



