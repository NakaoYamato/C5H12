#include "TerrainController.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "../../Library/Scene/Scene.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

#include <imgui.h>

// 生成時処理
void TerrainController::OnCreate()
{
    // 地形オブジェクトを生成
    _terrain = std::make_shared<Terrain>(Graphics::Instance().GetDevice());

	// 地形の初期化
    _terrain->SetStreamOut(true);
}
// 遅延更新処理
void TerrainController::LateUpdate(float elapsedTime)
{
	// 編集フラグが立っている場合は地形の再計算を行う
	if (_isEditing)
	{
        // 地形の初期化
        _terrain->SetStreamOut(true);
		_isEditing = false;
	}
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
            _isEditing = true; // パラメータマップを読み込んだら地形編集フラグを立てる
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
		// 地形の編集フラグを切り替えるチェックボックス
		ImGui::Checkbox(u8"地形編集", &_isEditing);
		// ワイヤーフレーム描画フラグを切り替えるチェックボックス
		ImGui::Checkbox(u8"ワイヤーフレーム描画", &_drawWireframe);
        // 地形のGUI描画
        _terrain->DrawGui();
    }
}
// パラメータマップの読み込み
void TerrainController::LoadParameterMap(const wchar_t* filePath)
{
    // テクスチャの読み込み
    GpuResourceManager::LoadTextureFromFile(
        Graphics::Instance().GetDevice(),
        filePath,
        _loadParameterMapSRV.ReleaseAndGetAddressOf(),
        nullptr);
}
