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
        "./Data/Shader/TerrainDeformAddPS.cso",
        _pixelShader.ReleaseAndGetAddressOf());
}
// 更新処理
void ColorAdditionBrush::Update(float elapsedTime, Vector3* intersectWorldPosition)
{
    if (intersectWorldPosition)
    {
        // 地形に接触していて左クリックしたら編集
        if (_INPUT_PRESSED("LeftClick"))
        {

        }
    }
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
void ColorAdditionBrush::DrawGui()
{
}
