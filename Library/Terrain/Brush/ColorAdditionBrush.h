#pragma once

#include "../../Library/Component/Terrain/TerrainDeformer.h"

class ColorAdditionBrush : public TerrainDeformerBrush
{
public:
	ColorAdditionBrush(TerrainDeformer* deformer);
	~ColorAdditionBrush() override = default;
	// –¼‘Oæ“¾
	const char* GetName() const override { return "ColorAdditionBrush"; }
	// XVˆ—
	void Update(float elapsedTime, Vector3* intersectWorldPosition);
	// •`‰æˆ—
	void Render(std::shared_ptr<Terrain> terrain,
		const RenderContext& rc,
		ID3D11ShaderResourceView** srv,
		uint32_t startSlot,
		uint32_t numViews);
	// GUI•`‰æ
	void DrawGui();
};