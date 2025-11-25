#include "TerrainController.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "../../Library/Scene/Scene.h"
#include "../../Library/Algorithm/Converter.h"

#include "TerrainEnvironmentController.h"

#include <filesystem>
#include <imgui.h>
#include <Mygui.h>

TerrainController::TerrainController(const std::string& serializePath)
{
    // 地形オブジェクトを生成
    _terrain = std::make_shared<Terrain>(Graphics::Instance().GetDevice(), serializePath);
}

// 生成時処理
void TerrainController::OnCreate()
{
	// 地形の初期化
    {
        std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());
        _terrain->UpdateTextures(GetActor()->GetScene()->GetTextureRenderer(),
            Graphics::Instance().GetDeviceContext());
    }

    // 地形の環境物配置情報からアクター生成
	auto objectLayout = _terrain->GetTerrainObjectLayout();
    for (const auto& [index, layout] : objectLayout->GetLayouts())
    {
		CreateEnvironment(index);
    }

	// 地形の頂点情報をエクスポートする
    GetActor()->GetScene()->GetTerrainRenderer().ExportVertices(
        _terrain.get(),
        GetActor()->GetTransform().GetMatrix());
	_editState = EditState::Editing;
}
// 更新処理
void TerrainController::Update(float elapsedTime)
{
}
// 遅延更新処理
void TerrainController::LateUpdate(float elapsedTime)
{
    // 地形の更新
    if (_terrain)
    {
        std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());
        bool result = _terrain->UpdateTextures(GetActor()->GetScene()->GetTextureRenderer(),
            Graphics::Instance().GetDeviceContext());

        // 編集が行われていた時
        if (result)
        {
            _editState = EditState::Editing;
        }
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
    // 編集状態がCompleteの場合は、未編集状態に変更
    if (_editState == EditState::Complete)
        _editState = EditState::None;

    // 編集状態がEditingの場合は、地形の頂点再計算とコライダーの再構築を行い、編集状態をCompleteに変更
    if (_editState == EditState::Editing)
    {
        GetActor()->GetScene()->GetTerrainRenderer().ExportVertices(
            _terrain.get(),
            GetActor()->GetTransform().GetMatrix());
        _editState = EditState::Complete;
    }
}
// 描画処理
void TerrainController::Render(const RenderContext& rc)
{
    if (_terrain)
    {
        GetActor()->GetScene()->GetTerrainRenderer().Draw(
            _terrain.get(),
            GetActor()->GetTransform().GetMatrix());
    }
}
// 影描画
void TerrainController::CastShadow(const RenderContext& rc)
{
    if (_terrain)
    {
        GetActor()->GetScene()->GetTerrainRenderer().DrawShadow(
            _terrain.get(),
            GetActor()->GetTransform().GetMatrix());
    }
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
                color.x = v1.parameter.z;
                color.y = v1.parameter.y;
                Debug::Renderer::AddVertex(v1.worldPosition, color);
                color.x = v2.parameter.z;
                color.y = v2.parameter.y;
                Debug::Renderer::AddVertex(v2.worldPosition, color);
                color.x = v2.parameter.z;
                color.y = v2.parameter.y;
                Debug::Renderer::AddVertex(v2.worldPosition, color);
                color.x = v3.parameter.z;
                color.y = v3.parameter.y;
                Debug::Renderer::AddVertex(v3.worldPosition, color);
                color.x = v3.parameter.z;
                color.y = v3.parameter.y;
                Debug::Renderer::AddVertex(v3.worldPosition, color);
                color.x = v1.parameter.z;
                color.y = v1.parameter.y;
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
                    const Vector3 p1 = wall.vertices[i];
                    const Vector3 p2 = wall.vertices[i + 1];
                    const Vector3 p3 = p1 + heightOffset;
                    const Vector3 p4 = p2 + heightOffset;
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
        if (ImGui::Button(u8"頂点再計算"))
            _editState = EditState::Editing;
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
        std::string resultPath = "";
        if (ImGui::SaveDialogBotton(u8"保存", &resultPath, ImGui::JsonFilter, "保存先を指定"))
        {
            // 地形の情報をJSONファイルに保存
            _terrain->SaveToFile(Graphics::Instance().GetDevice(), Graphics::Instance().GetDeviceContext(), resultPath);
        }
        if (ImGui::OpenDialogBotton(u8"読み込み", &resultPath, ImGui::JsonFilter))
        {
            // 地形の情報をJSONファイルから読み込み
            _terrain->LoadFromFile(Graphics::Instance().GetDevice(), resultPath);
            // 再計算
            _editState = EditState::Editing;
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
	// 親子関係設定
	actor->SetParent(GetActor().get());
    // 親のトランスフォームを反映しない
    actor->SetInheritParentTransform(false);
}
