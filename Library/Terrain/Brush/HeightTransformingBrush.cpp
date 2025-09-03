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
}
// 更新処理
void HeightTransformingBrush::Update(std::vector<std::shared_ptr<TerrainController>>& terrainControllers,
    float elapsedTime,
    Vector3* intersectWorldPosition)
{
    if (!intersectWorldPosition)
        return;

    for (auto& terrainController : terrainControllers)
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

        // 地形に接触していて左クリックしたら編集
        if (terrainController && _INPUT_PRESSED("LeftClick"))
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
    terrain->GetParameterMapFB()->Activate(rc.deviceContext);
    fullscreenQuad->Blit(
        rc.deviceContext,
        srv,
        startSlot, numViews,
        _pixelShader.Get()
    );
    terrain->GetParameterMapFB()->Deactivate(rc.deviceContext);
}
// GUI描画
void HeightTransformingBrush::DrawGui()
{
    TerrainDeformerBrush::DrawGui();
    ImGui::InputFloat(u8"高さ", &_brushPadding.y);
}
// タスクを登録
void HeightTransformingBrush::RegisterTask(std::weak_ptr<TerrainController> terrainController, const Vector2& uvPosition, float radius, float strength)
{
	TerrainDeformerBrush::RegisterTask(terrainController, uvPosition, radius, strength);
    // 地形に編集を適用
    terrainController.lock()->SetEditState(TerrainController::EditState::Editing);
}
