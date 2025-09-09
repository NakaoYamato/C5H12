#pragma once

#include "../../Library/Component/Terrain/TerrainDeformer.h"

class GrassTransformingBrush : public TerrainDeformerBrush
{
public:
	GrassTransformingBrush(TerrainDeformer* deformer);
	~GrassTransformingBrush() override = default;
	// 名前取得
	const char* GetName() const override { return u8"草変形"; }
	// タスクを登録
	void RegisterTask(std::weak_ptr<TerrainController> terrainController,
		const Vector2& uvPosition,
		float radius,
		float strength) override;
};