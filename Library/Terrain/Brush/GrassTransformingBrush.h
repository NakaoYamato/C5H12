#pragma once

#include "../../Library/Component/Terrain/TerrainDeformer.h"

class GrassTransformingBrush : public TerrainDeformerBrush
{
public:
	GrassTransformingBrush(TerrainDeformer* deformer);
	~GrassTransformingBrush() override = default;
	// –¼‘Oæ“¾
	const char* GetName() const override { return u8"‘•ÏŒ`"; }
	// •`‰æˆ—
	void Render(SpriteResource* fullscreenQuad,
		std::shared_ptr<Terrain> terrain,
		const RenderContext& rc,
		ID3D11ShaderResourceView** srv,
		uint32_t startSlot,
		uint32_t numViews) override;
	// ƒ^ƒXƒN‚ğ“o˜^
	void RegisterTask(std::weak_ptr<TerrainController> terrainController,
		const Vector2& uvPosition,
		float radius,
		float strength) override;
};