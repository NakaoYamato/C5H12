#include "TerrainController.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

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
    //rc.deviceContext->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::WireCullBack));
    if (_terrain)
    {
        // 地形の描画
        _terrain->Render(rc, GetActor()->GetTransform().GetMatrix(), Graphics::Instance().RenderingDeferred());
    }
    //rc.deviceContext->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));
}
// デバッグ描画
void TerrainController::DebugRender(const RenderContext& rc)
{
    if (_terrain)
    {
        auto& streamOutData = _terrain->GetStreamOutData();
        if (streamOutData.empty())
            return;
        for (size_t i = 0; i < streamOutData.size(); i += 3)
        {
            const auto& v1 = streamOutData[i + 0];
            const auto& v2 = streamOutData[i + 1];
            const auto& v3 = streamOutData[i + 2];
            Debug::Renderer::AddVertex(v1.worldPosition);
            Debug::Renderer::AddVertex(v2.worldPosition);
            Debug::Renderer::AddVertex(v2.worldPosition);
            Debug::Renderer::AddVertex(v3.worldPosition);
            Debug::Renderer::AddVertex(v3.worldPosition);
            Debug::Renderer::AddVertex(v1.worldPosition);
        }
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
