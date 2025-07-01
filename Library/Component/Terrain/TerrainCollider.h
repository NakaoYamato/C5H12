#pragma once

#include "../Collider/MeshCollider.h"
#include "TerrainController.h"

class TerrainCollider : public MeshCollider
{
public:
	TerrainCollider() {}
	~TerrainCollider() override {}
	// 名前取得
	const char* GetName() const override { return "TerrainCollider"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// コリジョンメッシュの再計算
	void RecalculateCollisionMesh() override;
private:
	std::weak_ptr<TerrainController> _terrainController; // 地形コントローラーへの参照
};