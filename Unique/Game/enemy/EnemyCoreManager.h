#pragma once
#include "EnemyCore.h"

class EnemyCoreManager
{
public:

	static EnemyCoreManager* GetInstance();

	void AddCore(const std::shared_ptr<EnemyCore>& core) {
		enemyList_[core.get()] = core;
	}

	std::shared_ptr<EnemyCore> GetCore(GameObject* object) { 
		return enemyList_[object]; 
	}

	void Clear() { enemyList_.clear(); }

private:

	EnemyCoreManager() = default;
	~EnemyCoreManager() = default;
	EnemyCoreManager(const EnemyCoreManager&) = delete;
	EnemyCoreManager& operator=(const EnemyCoreManager&) = delete;

	std::unordered_map<GameObject*, std::shared_ptr<EnemyCore>> enemyList_;

};



