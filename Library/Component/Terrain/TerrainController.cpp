#include "TerrainController.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "../../Library/Scene/Scene.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

#include <imgui.h>

// 開始処理
void TerrainController::Start()
{
    // 地形オブジェクトを生成
    _terrain = std::make_shared<Terrain>(Graphics::Instance().GetDevice());

    // テクスチャの読み込み
    GpuResourceManager::LoadTextureFromFile(
        Graphics::Instance().GetDevice(),
        L"./Data/Texture/Terrain/ParameterMap.dds",
        _loadParameterMapSRV.ReleaseAndGetAddressOf(),
        nullptr);
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
        if (_loadParameterMapSRV.Get())
        {
            _terrain->GetParameterMapFB()->ClearAndActivate(rc.deviceContext);
            GetActor()->GetScene()->GetTextureRenderer().Blit(
                rc.deviceContext,
                _loadParameterMapSRV.GetAddressOf(),
                0, 1
            );
            _terrain->GetParameterMapFB()->Deactivate(rc.deviceContext);

            _loadParameterMapSRV.Reset();
        }

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

        if (!_drawWireframe)
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
		ImGui::Checkbox(u8"ワイヤーフレーム描画", &_drawWireframe);
        // 地形のGUI描画
        _terrain->DrawGui();
    }
}
