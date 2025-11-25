#include "ColorAdditionBrush.h"

#include "../../Library/Input/Input.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "../../Library/Scene/Scene.h"

#include <imgui.h>

ColorAdditionBrush::ColorAdditionBrush(TerrainDeformer* deformer) :
	TerrainDeformerBrush(deformer)
{
    // 加算ブラシピクセルシェーダの読み込み
    GpuResourceManager::CreatePsFromCso(
        Graphics::Instance().GetDevice(),
        "./Data/Shader/HLSL/Terrain/Deform/TerrainDeformAddPS.cso",
        _pixelShader.ReleaseAndGetAddressOf());
	_brushPadding.x = 1.0f;
	_brushPadding.y = 1.0f;
}
// GUI描画
void ColorAdditionBrush::DrawGui()
{
    TerrainDeformerBrush::DrawGui();
    ImGui::DragFloat(u8"高さマップ影響度", &_brushPadding.x, 0.01f);
    ImGui::DragFloat(u8"色補正値", &_brushPadding.y, 0.01f);
}
