#pragma once
#include <list>
#include <memory>
#include "Graphics/Model.h"
#include <string>
#include <vector>
#include "Math/Transform.h"

class MapEditor
{
public:
	
	static MapEditor* GetInstance();

	void Initialize();

	void Edit();

	const std::string& kDirectoryPath_ = "./resources/EnemySpawn/";

//関数
private:

	void Create(const std::string& filename);

	void Save(const std::string& filename);

	void Close();

	void Load(const std::string& filename);

	void AddEnemy();

	bool CheckIsEmpty(const std::string& name);

	void AddTag(const std::string& tagname);

	//同名オブジェクトを探す
	std::string CheckSameName(std::string name, uint32_t addNumber = 0);

	//同名タグを探す
	bool CheckSameTag(const std::string& name);

//変数
private:


	//マップに配置されているオブジェクトの構造体
	struct MapObject {

		//オブジェクト名
		std::string objName;

		//モデル
		std::shared_ptr<ModelInstance> model;

		//トランスフォーム
		std::shared_ptr<Transform> transform;

		//オブジェクトの役割を表すタグ
		std::string tag;

		int32_t tagNumber = 0;

		bool isSelect = false;

	};

	std::list<std::shared_ptr<MapObject>> mapObjData_;

	std::string currentObject_;

	bool isOpenFile_ = false;

	Vector3 spawnPoint_ = { 0.0f,0.0f,0.0f };

	char name_[256];

	char fileName_[256];

	bool isSave_ = true;

	char tagName_[256];

	std::vector<std::string> tagData_ = { "Normal", "Barrier"};

	std::vector<const char*> tags_;

private:

	MapEditor() = default;
	~MapEditor() = default;
	MapEditor(const MapEditor&) = delete;
	const MapEditor& operator=(const MapEditor&) = delete;

};
