#include "map.h"
#include<cassert>
#include<stdio.h>

#include"Graphics/ImGuiManager.h"





void Map::Initialize() { 
	
	
	mapWorld_.translate = {-10.0f, 0.0f, 0.0f};


	//マップタイルによる座標設定
	for (int tileY = 0; tileY < mapTileNumY; tileY++) {
		for (int tileX = 0; tileX < mapTileNumX; tileX++) {
			if (mapTile[tileY][tileX] == Spike) {
				Transform world;
				world.translate = {tileWide * tileX, -tileWide * tileY, 0};
				
				world.parent = &mapWorld_;
				world_.push_back(world);


				//壁のX値設定
				if (tileX == 0) {
					wallX.min = world.translate.x - tileWide/2;
				}
				if (tileX == mapTileNumX - 1) {
					wallX.max = world.translate.x + tileWide/2;
				}
			}

			if (mapTile[tileY][tileX] == Player) {
				playerW_.translate = {tileWide * tileX, -tileWide * tileY, 0};
				
			}
		}
	}

	
}

void Map::Update() {
#ifdef _DEBUG
	ImGui::Begin("map");
	ImGui::DragFloat3("pos", &mapWorld_.translate.x, 0.01f);
	ImGui::End();
#endif // _DEBUG



	mapWorld_.UpdateMatrix();
	
}




bool Map::IsHitWall(const Vector3& playerpos,const float& wide) {
	if ((playerpos.x+wide)>wallX.max) {
		return true;
	}
	if ((playerpos.x - wide) < wallX.min) {
		return true;
	}

	//外に出ていなければfalse
	return false;
}


