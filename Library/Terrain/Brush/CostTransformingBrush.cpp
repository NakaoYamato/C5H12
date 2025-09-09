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
    // コスト編集ブラシピクセルシェーダの読み込み
    GpuResourceManager::CreatePsFromCso(
        Graphics::Instance().GetDevice(),
        "./Data/Shader/HLSL/Terrain/Deform/TerrainDeformCostPS.cso",
        _pixelShader.ReleaseAndGetAddressOf());
}
// タスクを登録
void CostTransformingBrush::RegisterTask(std::weak_ptr<TerrainController> terrainController, const Vector2& uvPosition, float radius, float strength)
{
    TerrainDeformerBrush::RegisterTask(terrainController, uvPosition, radius, strength);
    // 地形に編集を適用
    terrainController.lock()->SetEditState(TerrainController::EditState::Editing);
}
