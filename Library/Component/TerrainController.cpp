#include "TerrainController.h"

#include "../../Library/Graphics/Graphics.h"

// 開始処理
void TerrainController::Start()
{
    // 地形オブジェクトを生成
    _terrain = std::make_shared<Terrain>(Graphics::Instance().GetDevice());
}
// 更新処理
void TerrainController::Update(float elapsedTime)
{
}
// 描画処理
void TerrainController::Render(const RenderContext& rc)
{
    if (_terrain)
    {
        // 地形の描画
        _terrain->Render(rc, GetActor()->GetTransform().GetMatrix(), Graphics::Instance().RenderingDeferred());
    }
}
// GUI描画
void TerrainController::DrawGui()
{
    if (_terrain)
    {
        // 地形のGUI描画
        _terrain->DrawGui();
    }
}
