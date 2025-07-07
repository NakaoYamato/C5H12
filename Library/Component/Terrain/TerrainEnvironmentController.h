#pragma once

#include "TerrainController.h"

class TerrainEnvironmentController : public Component
{
public:
	TerrainEnvironmentController(std::weak_ptr<Terrain> terrain, int id) :
		_terrain(terrain),
		_objectID(id)
	{
	}
	~TerrainEnvironmentController() override = default;
	// 名前取得
	const char* GetName() const override { return "TerrainEnvironmentController"; }
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

private:
	// 地形への参照
	std::weak_ptr<Terrain> _terrain;
	// 自身の配置番号
	int _objectID = -1;
	// 地形の配置情報で上書きするか
	bool _overwrite = false;
};