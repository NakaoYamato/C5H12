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
		if (_loadDataMapSRV.Get())
		{
			_terrain->GetDataMapFB()->ClearAndActivate(rc.deviceContext);
			GetActor()->GetScene()->GetTextureRenderer().Blit(
				rc.deviceContext,
				_loadDataMapSRV.GetAddressOf(),
				0, 1
			);
			_terrain->GetDataMapFB()->Deactivate(rc.deviceContext);
			_loadDataMapSRV.Reset();
			_isEditing = true; // データマップを読み込んだら地形編集フラグを立てる
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
		_terrain->DebugRender(GetActor()->GetTransform().GetMatrix());
    }
}
// GUI描画
void TerrainController::DrawGui()
{
    if (_terrain)
    {
		// 地形の編集フラグを切り替えるチェックボックス
		ImGui::Checkbox(u8"地形編集", &_isEditing);
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
// データマップの読み込み
void TerrainController::LoadDataMap(const wchar_t* filePath)
{
    // テクスチャの読み込み
    GpuResourceManager::LoadTextureFromFile(
        Graphics::Instance().GetDevice(),
        filePath,
        _loadDataMapSRV.ReleaseAndGetAddressOf(),
        nullptr);
}
