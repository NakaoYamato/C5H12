#include "GrassTransformingBrush.h"

#include "../../Library/Input/Input.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"
#include "../../Library/Scene/Scene.h"

#include <imgui.h>

GrassTransformingBrush::GrassTransformingBrush(TerrainDeformer* deformer) :
    TerrainDeformerBrush(deformer)
{
    // 草編集ブラシピクセルシェーダの読み込み
    GpuResourceManager::CreatePsFromCso(
        Graphics::Instance().GetDevice(),
        "./Data/Shader/HLSL/Terrain/Deform/TerrainDeformGrassPS.cso",
        _pixelShader.ReleaseAndGetAddressOf());
}

void GrassTransformingBrush::Render(SpriteResource* fullscreenQuad,
    std::shared_ptr<Terrain> terrain,
    const RenderContext & rc,
    ID3D11ShaderResourceView * *srv,
    uint32_t startSlot,
    uint32_t numViews)
{
    terrain->GetParameterMapFB()->Activate(rc.deviceContext);
    fullscreenQuad->Blit(
        rc.deviceContext,
        srv,
        startSlot, numViews,
        _pixelShader.Get()
    );
    terrain->GetParameterMapFB()->Deactivate(rc.deviceContext);
}
// タスクを登録
void GrassTransformingBrush::RegisterTask(std::weak_ptr<TerrainController> terrainController, const Vector2& uvPosition, float radius, float strength)
{
    TerrainDeformerBrush::RegisterTask(terrainController, uvPosition, radius, strength);
    // 地形に編集を適用
    terrainController.lock()->SetEditState(TerrainController::EditState::Editing);
}
