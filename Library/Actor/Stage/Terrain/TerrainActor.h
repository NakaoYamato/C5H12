#pragma once

#include "../../Library/Actor/Actor.h"

class TerrainActor : public Actor
{
public:
	TerrainActor(const std::string& terrainPath = "./Data/Terrain/Save/002.json",
		const Vector3& startPosition = Vector3::Zero) :
		_terrainPath(terrainPath)
	{
		GetTransform().SetPosition(startPosition);
	}
	~TerrainActor()override {}

	// 生成時処理
	void OnCreate() override;
private:
	// 地形の保存パス
	std::string _terrainPath;
};