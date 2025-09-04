#pragma once

#include "../../Library/Actor/Actor.h"

class StageActor : public Actor
{
public:
	StageActor(int areNumber = 0, const std::string& terrainPath = "./Data/Terrain/Save/002.json", const Vector3& startPosition = Vector3::Zero) :
		_areaNumber(areNumber),
		_terrainPath(terrainPath)
	{
		GetTransform().SetPosition(startPosition);
	}
	~StageActor()override {}

	// 生成時処理
	void OnCreate() override;
private:
	// 地形の保存パス
	std::string _terrainPath;
	// エリア番号
	int _areaNumber = -1;
};