#include "TerrainController.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "../../Library/Scene/Scene.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"
#include "../../Library/Algorithm/Converter.h"

#include "TerrainEnvironmentController.h"

#include <filesystem>
#include <imgui.h>

TerrainController::TerrainController(const std::string& serializePath)
{
    // 地形オブジェクトを生成
    _terrain = std::make_shared<Terrain>(Graphics::Instance().GetDevice(), serializePath);
}

// 生成時処理
void TerrainController::OnCreate()
{
	// 地形の初期化
    _terrain->SetStreamOut(true);
    // 地形の環境物配置情報からアクター生成
	auto objectLayout = _terrain->GetTerrainObjectLayout();
    for (const auto& [index, layout] : objectLayout->GetLayouts())
    {
		CreateEnvironment(index);
    }
	_isEditing = true;
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
    if (_recreateEnvironment)
    {
        // 地形の環境物配置情報からアクター生成
        auto objectLayout = _terrain->GetTerrainObjectLayout();
        for (const auto& [index, layout] : objectLayout->GetLayouts())
        {
            CreateEnvironment(index);
        }
		_recreateEnvironment = false;
    }
}
// 描画処理
void TerrainController::Render(const RenderContext& rc)
{
    //rc.deviceContext->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::WireCullBack));
    if (_terrain)
    {
        // 地形の描画
        _terrain->Render(GetActor()->GetScene()->GetTextureRenderer(),
            rc,
            GetActor()->GetTransform().GetMatrix(),
            Graphics::Instance().RenderingDeferred());
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
			const DirectX::XMFLOAT4X4& world = GetActor()->GetTransform().GetMatrix();
			for (const auto& wall : _terrain->GetTransparentWall()->GetWalls())
			{
				size_t pointCount = wall.vertices.size();
				if (pointCount <= 1)
					continue;
				Vector3 heightOffset = Vector3(0.0f, wall.height, 0.0f);
				for (size_t i = 0; i < pointCount - 1; i++)
				{
                    const Vector3& p1 = wall.vertices[i].TransformCoord(world);
                    const Vector3& p2 = wall.vertices[i + 1].TransformCoord(world);
                    const Vector3& p3 = p1 + heightOffset;
                    const Vector3& p4 = p2 + heightOffset;
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
					Vector3 center = (p1 + p2 + p3 + p4) / 4.0f;
					Vector3 normal = (p2 - p1).Cross(p3 - p1).Normalize();
					// 法線の向きがカメラ方向なら青色、逆方向なら赤色
					if (normal.Dot(rc.camera->GetEye() - center) < 0.0f)
                        color = Vector4::Red;
					else
                        color = Vector4::Blue;
					Debug::Renderer::AddVertex(center, color);
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
        if (ImGui::Button(u8"環境物のリセット"))
        {
			for (auto& obj : _environmentObjects)
			{
                if (obj.lock())
                {
					obj.lock()->Remove();
                }
			}
            _environmentObjects.clear();
			_recreateEnvironment = true;
        }
        ImGui::Separator();
        // 地形のGUI描画
        _terrain->DrawGui(Graphics::Instance().GetDevice(), Graphics::Instance().GetDeviceContext());
    }
}
// 環境物の生成
void TerrainController::CreateEnvironment(int layoutID)
{
	if (!_terrain)
		return;
    // 描画用アクター生成
    std::string name = GetActor()->GetName();
    name += "Obj" + std::to_string(layoutID);
    auto actor = GetActor()->GetScene()->RegisterActor<Actor>(name, ActorTag::Stage);
    actor->AddComponent<TerrainEnvironmentController>(_terrain, layoutID, GetActor()->GetTransform().GetMatrix());
	_environmentObjects.push_back(actor);
}
