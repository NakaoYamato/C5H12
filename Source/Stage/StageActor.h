#pragma once

#include "../../Library/Actor/Actor.h"

class StageActor : public Actor
{
public:
	StageActor(const std::string& terrainPath = "./Data/Terrain/Save/002.json", const Vector3& startPosition = Vector3::Zero) :
		_terrainPath(terrainPath)
	{
		GetTransform().SetPosition(startPosition);
	}
	~StageActor()override {}

	// 生成時処理
	void OnCreate() override;
private:
	std::string _terrainPath; // 地形の保存パス
};