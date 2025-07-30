#pragma once

#include "../../Library/Component/Terrain/TerrainDeformer.h"

class CostTransformingBrush : public TerrainDeformerBrush
{
public:
	CostTransformingBrush(TerrainDeformer* deformer);
	~CostTransformingBrush() override = default;
	// 名前取得
	const char* GetName() const override { return u8"コスト変形"; }
	// 描画処理
	void Render(SpriteResource* fullscreenQuad,
		std::shared_ptr<Terrain> terrain,
		const RenderContext& rc,
		ID3D11ShaderResourceView** srv,
		uint32_t startSlot,
		uint32_t numViews) override;
	// タスクを登録
	void RegisterTask(std::weak_ptr<TerrainController> terrainController,
		const Vector2& uvPosition,
		float radius,
		float strength) override;
};