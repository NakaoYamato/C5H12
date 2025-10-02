#include "HeightTransformingBrush.h"

#include "../../Library/Input/Input.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"
#include "../../Library/Scene/Scene.h"

#include <imgui.h>

HeightTransformingBrush::HeightTransformingBrush(TerrainDeformer* deformer) :
    TerrainDeformerBrush(deformer)
{
    // 高さ変形ブラシピクセルシェーダの読み込み
    GpuResourceManager::CreatePsFromCso(
        Graphics::Instance().GetDevice(),
        "./Data/Shader/HLSL/Terrain/Deform/TerrainDeformHeightPS.cso",
        _pixelShader.ReleaseAndGetAddressOf());
    // でこぼこブラシピクセルシェーダの読み込み
    GpuResourceManager::CreatePsFromCso(
        Graphics::Instance().GetDevice(),
        "./Data/Shader/HLSL/Terrain/Deform/TerrainDeformBumpyHeightPS.cso",
        _bumpyPixelShader.ReleaseAndGetAddressOf());
}
// 更新処理
void HeightTransformingBrush::Update(std::vector<std::shared_ptr<TerrainController>>& terrainControllers,
    float elapsedTime,
    Vector3* intersectWorldPosition)
{
    if (!intersectWorldPosition)
        return;

    if (!_useBumpyShader)
    {
        if (_INPUT_TRIGGERD("LeftClick"))
        {
            // ワールド座標が目標高さより高い場合パディングを変更
            if (intersectWorldPosition->y > _brushPadding.y)
            {
                _brushPadding.x = -1.0f;
            }
            else // それ以外はパディングをリセット
            {
                _brushPadding.x = 1.0f;
            }
        }
    }

    for (auto& terrainController : terrainControllers)
    {
        if (!terrainController)
            continue;

        // 左クリックしたら編集
        if (_INPUT_PRESSED("LeftClick"))
        {
            Vector3 uv = intersectWorldPosition->TransformCoord(terrainController->GetActor()->GetTransform().GetMatrixInverse());
            Vector2 intersectUVPosition{};
            intersectUVPosition.x = (uv.x + 1.0f) / 2.0f;
            intersectUVPosition.y = (-uv.z + 1.0f) / 2.0f;

            RegisterTask(terrainController,
                intersectUVPosition,
                _brushRadius / terrainController->GetActor()->GetTransform().GetScale().x,
                _brushStrength * elapsedTime);
        }

        if (_INPUT_RELEASED("LeftClick"))
        {
            // 地形に編集を適用
            terrainController->SetEditState(TerrainController::EditState::Editing);
        }
    }
}
// 描画処理
void HeightTransformingBrush::Render(SpriteResource* fullscreenQuad, 
    std::shared_ptr<Terrain> terrain,
    const RenderContext& rc, 
    ID3D11ShaderResourceView** srv,
    uint32_t startSlot, 
    uint32_t numViews)
{
    if (_useBumpyShader)
    {
        fullscreenQuad->Blit(
            rc.deviceContext,
            srv,
            startSlot, numViews,
            _bumpyPixelShader.Get()
        );
    }
    else
    {
        fullscreenQuad->Blit(
            rc.deviceContext,
            srv,
            startSlot, numViews,
            _pixelShader.Get()
        );
    }
}
// GUI描画
void HeightTransformingBrush::DrawGui()
{
    TerrainDeformerBrush::DrawGui();
    ImGui::DragFloat(u8"高さ", &_brushPadding.y);
    ImGui::Checkbox(u8"でこぼこブラシを使用", &_useBumpyShader);
    if (_useBumpyShader)
        ImGui::DragFloat(u8"でこぼこシード値", &_brushPadding.x, 0.1f, 0.0f, 10.0f);
}
