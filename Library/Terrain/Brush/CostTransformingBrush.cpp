#include "CostTransformingBrush.h"

#include "../../Library/Input/Input.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"
#include "../../Library/Scene/Scene.h"

#include <imgui.h>

CostTransformingBrush::CostTransformingBrush(TerrainDeformer* deformer) :
    TerrainDeformerBrush(deformer)
{
    // 高さ変形ブラシピクセルシェーダの読み込み
    GpuResourceManager::CreatePsFromCso(
        Graphics::Instance().GetDevice(),
        "./Data/Shader/TerrainDeformCostPS.cso",
        _pixelShader.ReleaseAndGetAddressOf());
}
// 描画処理
void CostTransformingBrush::Render(std::shared_ptr<Terrain> terrain, const RenderContext& rc, ID3D11ShaderResourceView** srv, uint32_t startSlot, uint32_t numViews)
{
    terrain->GetParameterMapFB()->Activate(rc.deviceContext);
    _deformer->GetActor()->GetScene()->GetTextureRenderer().Blit(
        rc.deviceContext,
        srv,
        startSlot, numViews,
        _pixelShader.Get()
    );
    terrain->GetParameterMapFB()->Deactivate(rc.deviceContext);
}
