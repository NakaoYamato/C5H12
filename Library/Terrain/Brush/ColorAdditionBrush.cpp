#include "ColorAdditionBrush.h"

#include "../../Library/Input/Input.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"
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
}
