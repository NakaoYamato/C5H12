#pragma once

#include "../../Library/Component/Terrain/TerrainDeformer.h"

class HeightTransformingBrush : public TerrainDeformerBrush
{
public:
	HeightTransformingBrush(TerrainDeformer* deformer);
	~HeightTransformingBrush() override = default;
	// 名前取得
	const char* GetName() const override { return u8"高さ変形"; }
	// 更新処理
	void Update(std::vector<std::shared_ptr<TerrainController>>& terrainControllers,
		float elapsedTime,
		Vector3* intersectWorldPosition) override;
	// 描画処理
	void Render(SpriteResource* fullscreenQuad, 
		std::shared_ptr<Terrain> terrain,
		const RenderContext& rc,
		ID3D11ShaderResourceView** srv,
		uint32_t startSlot,
		uint32_t numViews) override;
	// GUI描画
	void DrawGui() override;
private:
	// でこぼこシェーダ
    Microsoft::WRL::ComPtr<ID3D11PixelShader> _bumpyPixelShader;
	// でこぼこシェーダ使用フラグ
    bool _useBumpyShader = false;
};