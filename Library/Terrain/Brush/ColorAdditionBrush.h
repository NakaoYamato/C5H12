#pragma once

#include "../../Library/Component/Terrain/TerrainDeformer.h"

class ColorAdditionBrush : public TerrainDeformerBrush
{
public:
	ColorAdditionBrush(TerrainDeformer* deformer);
	~ColorAdditionBrush() override = default;
	// –¼‘Oæ“¾
	const char* GetName() const override { return u8"F‰ÁZ"; }
};