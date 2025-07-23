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
        "./Data/Shader/TerrainDeformAddPS.cso",
        _pixelShader.ReleaseAndGetAddressOf());
}
// 描画処理
void ColorAdditionBrush::Render(std::shared_ptr<Terrain> terrain,
    const RenderContext& rc,
    ID3D11ShaderResourceView** srv,
    uint32_t startSlot,
    uint32_t numViews)
{
    terrain->GetMaterialMapFB()->Activate(rc.deviceContext);
    _deformer->GetActor()->GetScene()->GetTextureRenderer().Blit(
        rc.deviceContext,
        srv,
        startSlot, numViews,
        _pixelShader.Get()
    );
    terrain->GetMaterialMapFB()->Deactivate(rc.deviceContext);
}
// GUI描画
void ColorAdditionBrush::DrawGui(std::shared_ptr<Terrain> terrain)
{
	TerrainDeformerBrush::DrawGui(terrain);
    ImGui::DragFloat(u8"高さテクスチャの影響度", &_padding.x, 0.01f, 0.0f);
}
