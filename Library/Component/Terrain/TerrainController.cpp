#include "TerrainController.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "../../Library/Scene/Scene.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"
#include "../../Library/Algorithm/Converter.h"

#include <filesystem>
#include <imgui.h>

// 生成時処理
void TerrainController::OnCreate()
{
    // 地形オブジェクトを生成
    _terrain = std::make_shared<Terrain>(Graphics::Instance().GetDevice());
	_terrain->LoadFromFile(_serializePath);

    // Terrainからテクスチャパスを取得して、テクスチャを読み込む
	LoadTerrainTextures();

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
        if (!_assignTextures)
        {
            if (_baseColorTextureSRV)
            {
                _terrain->GetMaterialMapFB()->ClearAndActivate(Terrain::BaseColorTextureIndex, rc.deviceContext);
                GetActor()->GetScene()->GetTextureRenderer().Blit(
                    rc.deviceContext,
                    _baseColorTextureSRV.GetAddressOf(),
                    0, 1
                );
                _terrain->GetMaterialMapFB()->Deactivate(rc.deviceContext);
                _baseColorTextureSRV.Reset();
            }
            if (_normalTextureSRV)
            {
                _terrain->GetMaterialMapFB()->ClearAndActivate(Terrain::NormalTextureIndex, rc.deviceContext);
                GetActor()->GetScene()->GetTextureRenderer().Blit(
                    rc.deviceContext,
                    _normalTextureSRV.GetAddressOf(),
                    0, 1
                );
                _terrain->GetMaterialMapFB()->Deactivate(rc.deviceContext);
                _normalTextureSRV.Reset();
            }
            if (_parameterTextureSRV)
            {
                _terrain->GetParameterMapFB()->ClearAndActivate(rc.deviceContext);
                GetActor()->GetScene()->GetTextureRenderer().Blit(
                    rc.deviceContext,
                    _parameterTextureSRV.GetAddressOf(),
                    0, 1
                );
                _terrain->GetParameterMapFB()->Deactivate(rc.deviceContext);
                _parameterTextureSRV.Reset();
            }

            _assignTextures = true;
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
        std::wstring filepath;
        auto LoadGUI = [&](const char* title, std::wstring* resultPath) -> bool
            {
                if (ImGui::Button(title))
                {
                    // ダイアログを開く
                    std::string filepath;
                    std::string currentDirectory;
                    const char* filter = "Texture Files(*.dds)\0*.dds;\0All Files(*.*)\0*.*;\0\0";
                    Debug::Dialog::DialogResult result = Debug::Dialog::OpenFileName(filepath, currentDirectory, filter);
                    // ファイルを選択したら
                    if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
                    {
                        // 相対パス取得
                        std::filesystem::path path =
                            std::filesystem::relative(filepath, currentDirectory);
						*resultPath = path.wstring();
                        return true;
                    }
                }
                return false;
            };
        if (LoadGUI(u8"基本色テクスチャ読み込み", &filepath))
        {
            GpuResourceManager::LoadTextureFromFile(
                Graphics::Instance().GetDevice(),
                filepath.c_str(),
                _baseColorTextureSRV.ReleaseAndGetAddressOf(),
                nullptr);
            _terrain->SetBaseColorTexturePath(filepath.c_str());
            _assignTextures = false; // テクスチャを再割り当てする
        }
        if (LoadGUI(u8"法線テクスチャ読み込み", &filepath))
        {
            GpuResourceManager::LoadTextureFromFile(
                Graphics::Instance().GetDevice(),
                filepath.c_str(),
                _normalTextureSRV.ReleaseAndGetAddressOf(),
                nullptr);
            _terrain->SetNormalTexturePath(filepath.c_str());
            _assignTextures = false; // テクスチャを再割り当てする
        }
        if (LoadGUI(u8"パラメータマップ読み込み", &filepath))
        {
            GpuResourceManager::LoadTextureFromFile(
                Graphics::Instance().GetDevice(),
                filepath.c_str(),
                _parameterTextureSRV.ReleaseAndGetAddressOf(),
                nullptr);
            _terrain->SetParameterTexturePath(filepath.c_str());
            _assignTextures = false; // テクスチャを再割り当てする
        }
        ImGui::Separator();
        if (ImGui::Button(u8"基本色テクスチャ書き出し"))
        {
            // ダイアログを開く
            char filename[256] = { 0 };
            const char* filter = "Texture Files(*.dds)\0*.dds;\0All Files(*.*)\0*.*;\0\0";
            Debug::Dialog::DialogResult result = Debug::Dialog::SaveFileName(
                filename,
                sizeof(filename),
                filter,
                nullptr,
                "dds");
            if (result == Debug::Dialog::DialogResult::OK)
            {
                _terrain->SaveBaseColorTexture(Graphics::Instance().GetDevice(),
                    Graphics::Instance().GetDeviceContext(),
                    ToWString(filename).c_str());
            }
        }
        if (ImGui::Button(u8"法線テクスチャ書き出し"))
        {
            // ダイアログを開く
            char filename[256] = { 0 };
            const char* filter = "Texture Files(*.dds)\0*.dds;\0All Files(*.*)\0*.*;\0\0";
            Debug::Dialog::DialogResult result = Debug::Dialog::SaveFileName(
                filename,
                sizeof(filename),
                filter,
                nullptr,
                "dds");
            if (result == Debug::Dialog::DialogResult::OK)
            {
                _terrain->SaveNormalTexture(Graphics::Instance().GetDevice(),
                    Graphics::Instance().GetDeviceContext(),
                    ToWString(filename).c_str());
            }
        }
		if (ImGui::Button(u8"パラメータマップ書き出し"))
		{
			// ダイアログを開く
			char filename[256] = { 0 };
			const char* filter = "Texture Files(*.dds)\0*.dds;\0All Files(*.*)\0*.*;\0\0";
			Debug::Dialog::DialogResult result = Debug::Dialog::SaveFileName(
                filename,
                sizeof(filename), 
                filter,
                nullptr,
                "dds");
			if (result == Debug::Dialog::DialogResult::OK)
			{
                _terrain->SaveParameterMap(Graphics::Instance().GetDevice(),
                    Graphics::Instance().GetDeviceContext(),
                    ToWString(filename).c_str());
			}
		}
        ImGui::Separator();
        if (ImGui::Button(u8"シリアライズ"))
        {
            // ダイアログを開く
            char filename[256] = { 0 };
            const char* filter = "Texture Files(*.json)\0*.json;\0All Files(*.*)\0*.*;\0\0";
            Debug::Dialog::DialogResult result = Debug::Dialog::SaveFileName(
                filename,
                sizeof(filename),
                filter,
                nullptr,
                "json");
            if (result == Debug::Dialog::DialogResult::OK)
            {
                _terrain->SaveToFile(filename);
            }
        }
		if (ImGui::Button(u8"デシリアライズ"))
		{
            // ダイアログを開く
            std::string filepath;
            std::string currentDirectory;
            const char* filter = "Texture Files(*.json)\0*.json;\0All Files(*.*)\0*.*;\0\0";
            Debug::Dialog::DialogResult result = Debug::Dialog::OpenFileName(filepath, currentDirectory, filter);
            // ファイルを選択したら
            if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
			{
                // 相対パス取得
                std::filesystem::path path =
                    std::filesystem::relative(filepath, currentDirectory);
				_terrain->LoadFromFile(path.string());
                LoadTerrainTextures();
			}
		}
    }
}
// Terrainからシリアライズデータを読み込む
void TerrainController::LoadTerrainTextures()
{
    if (_terrain->GetBaseColorTexturePath().empty())
    {
        GpuResourceManager::LoadTextureFromFile(
            Graphics::Instance().GetDevice(),
            L"./Data/Terrain/Texture/Material/001_COLOR.png",
            _baseColorTextureSRV.ReleaseAndGetAddressOf(),
            nullptr);
    }
    else
    {
        GpuResourceManager::LoadTextureFromFile(
            Graphics::Instance().GetDevice(),
            _terrain->GetBaseColorTexturePath().c_str(),
            _baseColorTextureSRV.ReleaseAndGetAddressOf(),
            nullptr);
    }
    if (_terrain->GetNormalTexturePath().empty())
    {
        GpuResourceManager::LoadTextureFromFile(
            Graphics::Instance().GetDevice(),
            L"./Data/Terrain/Texture/Material/001_NORMAL.png",
            _normalTextureSRV.ReleaseAndGetAddressOf(),
            nullptr);
    }
    else
    {
        GpuResourceManager::LoadTextureFromFile(
            Graphics::Instance().GetDevice(),
            _terrain->GetNormalTexturePath().c_str(),
            _normalTextureSRV.ReleaseAndGetAddressOf(),
            nullptr);
    }
    if (_terrain->GetParameterTexturePath().empty())
    {
        GpuResourceManager::LoadTextureFromFile(
            Graphics::Instance().GetDevice(),
            L"./Data/Terrain/Texture/Parameter/ParameterMap.dds",
            _parameterTextureSRV.ReleaseAndGetAddressOf(),
            nullptr);
    }
    else
    {
        GpuResourceManager::LoadTextureFromFile(
            Graphics::Instance().GetDevice(),
            _terrain->GetParameterTexturePath().c_str(),
            _parameterTextureSRV.ReleaseAndGetAddressOf(),
            nullptr);
    }
    _assignTextures = false; // テクスチャを再割り当てする
}
