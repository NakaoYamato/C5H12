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
        // ストリームアウトデータの描画
        if (_drawStreamOut)
        {
            auto& streamOutData = _terrain->GetStreamOutData();
            if (streamOutData.empty())
                return;

            for (size_t i = 0; i < streamOutData.size(); i += 3)
            {
                const auto& v1 = streamOutData[i + 0];
                const auto& v2 = streamOutData[i + 1];
                const auto& v3 = streamOutData[i + 2];
                Vector4 color = Vector4::Red;
                color.x = Vector4::Red.x * v1.cost;
                Debug::Renderer::AddVertex(v1.worldPosition, color);
                color.x = Vector4::Red.x * v2.cost;
                Debug::Renderer::AddVertex(v2.worldPosition, color);
                color.x = Vector4::Red.x * v2.cost;
                Debug::Renderer::AddVertex(v2.worldPosition, color);
                color.x = Vector4::Red.x * v3.cost;
                Debug::Renderer::AddVertex(v3.worldPosition, color);
                color.x = Vector4::Red.x * v3.cost;
                Debug::Renderer::AddVertex(v3.worldPosition, color);
                color.x = Vector4::Red.x * v1.cost;
                Debug::Renderer::AddVertex(v1.worldPosition, color);
            }
        }

		// 透明壁の描画
		if (_drawTransparentWall)
		{
			for (const auto& wall : _terrain->GetTransparentWalls())
			{
				size_t pointCount = wall.points.size();
				if (pointCount <= 1)
					continue;
				Vector3 heightOffset = Vector3(0.0f, wall.height, 0.0f);
				for (size_t i = 0; i < pointCount - 1; i++)
				{
					const Vector3& p1 = wall.points[i];
					const Vector3& p2 = wall.points[i + 1];
					const Vector3& p3 = wall.points[i] + heightOffset;
					const Vector3& p4 = wall.points[i + 1] + heightOffset;
					Vector4 color = Vector4::Green;
					Debug::Renderer::AddVertex(p1, color);
					Debug::Renderer::AddVertex(p2, color);
					Debug::Renderer::AddVertex(p1, color);
					Debug::Renderer::AddVertex(p3, color);
					Debug::Renderer::AddVertex(p2, color);
					Debug::Renderer::AddVertex(p4, color);
					Debug::Renderer::AddVertex(p3, color);
					Debug::Renderer::AddVertex(p4, color);
					// 法線の描画
					color = Vector4::Blue;
					Vector3 center = (p1 + p2 + p3 + p4) / 4.0f;
					Debug::Renderer::AddVertex(center, color);
					Vector3 normal = (p2 - p1).Cross(p3 - p1).Normalize();
					Debug::Renderer::AddVertex(center + normal, color);
				}
			}
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
		// ストリームアウトデータの描画フラグを切り替えるチェックボックス
		ImGui::Checkbox(u8"ストリームアウトデータ描画", &_drawStreamOut);
		// 透明壁の描画フラグを切り替えるチェックボックス
		ImGui::Checkbox(u8"透明壁描画", &_drawTransparentWall);
        ImGui::Separator();
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
