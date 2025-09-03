#pragma once

#include "../../Library/Component/Terrain/TerrainDeformer.h"

class HeightTransformingBrush : public TerrainDeformerBrush
{
public:
	HeightTransformingBrush(TerrainDeformer* deformer);
	~HeightTransformingBrush() override = default;
	// –¼‘Oæ“¾
	const char* GetName() const override { return u8"‚‚³•ÏŒ`"; }
	// XVˆ—
	void Update(std::vector<std::shared_ptr<TerrainController>>& terrainControllers,
		float elapsedTime,
		Vector3* intersectWorldPosition) override;
	// •`‰æˆ—
	void Render(SpriteResource* fullscreenQuad, 
		std::shared_ptr<Terrain> terrain,
		const RenderContext& rc,
		ID3D11ShaderResourceView** srv,
		uint32_t startSlot,
		uint32_t numViews) override;
	// GUI•`‰æ
	void DrawGui() override;
	// ƒ^ƒXƒN‚ğ“o˜^
	void RegisterTask(std::weak_ptr<TerrainController> terrainController,
		const Vector2& uvPosition,
		float radius,
		float strength) override;
};