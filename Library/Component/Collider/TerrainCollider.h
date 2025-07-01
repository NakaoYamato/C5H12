#pragma once

#include "MeshCollider.h"

class TerrainCollider : public MeshCollider
{
public:
	TerrainCollider() {}
	~TerrainCollider() override {}
	// 名前取得
	const char* GetName() const override { return "TerrainCollider"; }
	// 開始処理
	void Start() override;
	// コリジョンメッシュの再計算
	void RecalculateCollisionMesh() override;
};