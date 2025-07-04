#include "TerrainCollisionMaker.h"

#include "../../Library/Input/Input.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Scene/Scene.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"
#include "../../Library/Collision/CollisionMath.h"

#include <imgui.h>
#include <ImGuizmo.h>

// 開始処理
void TerrainCollisionMaker::Start()
{
    // 地形コントローラーを取得
    _terrainController = GetActor()->GetComponent<TerrainController>();
}
// 更新処理
void TerrainCollisionMaker::Update(float elapsedTime)
{
    // 地形コントローラーが取得できていない場合は何もしない
    if (!_terrainController.lock())
        return;
	// 編集フラグが立っていない場合は何もしない
	if (!_isEditing)
		return;
#ifdef USE_IMGUI
    //	ウィンドウにフォーカス中の場合は処理しない
    if (ImGui::IsAnyItemActive() || ImGui::IsAnyItemFocused() || ImGui::IsAnyItemHovered())
    {
		// 前フレームのGUI操作フラグをオンにする
		_wasGuiActive = true;
        return;
    }
	// 前フレームのGUI操作フラグがオンの場合は左クリックを押されるまで処理しない
	if (_wasGuiActive)
	{
        if (_INPUT_RELEASED("LeftClick"))
            _wasGuiActive = false;
		return;
	}
#endif
    auto terrain = _terrainController.lock()->GetTerrain().lock();
    if (!terrain)
        return;

    // アクターのギズモ使用フラグを常にオフ
	GetActor()->SetUseGuizmoFlag(false);

    _clickScreenPoint.x = _INPUT_VALUE("MousePositionX");
    _clickScreenPoint.y = _INPUT_VALUE("MousePositionY");
    float screenWidth = Graphics::Instance().GetScreenWidth();
    float screenHeight = Graphics::Instance().GetScreenHeight();
    auto& view = GetActor()->GetScene()->GetMainCamera()->GetView();
    auto& projection = GetActor()->GetScene()->GetMainCamera()->GetProjection();

	if (_state == State::None)
	{
        _intersectState = IntersectState::None;
        // マウス座標から地形との当たり判定
        Vector3 rayStart = Vector3::Unproject(Vector3(_clickScreenPoint.x, _clickScreenPoint.y, 0.0f), screenWidth, screenHeight, view, projection);
        Vector3 rayEnd = Vector3::Unproject(Vector3(_clickScreenPoint.x, _clickScreenPoint.y, 1.0f), screenWidth, screenHeight, view, projection);
        Vector3 rayDir = (rayEnd - rayStart).Normalize();
        // 地形との交差判定
        bool isIntersect = terrain->Raycast(
            GetActor()->GetTransform().GetMatrix(),
            rayStart,
            rayDir,
            _rayLength,
            &_intersectionWorldPoint);
		if (isIntersect)
            _intersectState = IntersectState::Terrain;

		// 透明壁のポイントとの交差判定
		for (size_t i = 0; i < terrain->GetTransparentWalls().size(); i++)
		{
			const auto& wall = terrain->GetTransparentWalls()[i];
			for (size_t j = 0; j < wall.points.size(); j++)
			{
				const Vector3& point = wall.points[j];
				// 透明壁のポイントとの交差判定
				if (Collision3D::IntersectRayVsSphere(
					rayStart, 
                    rayDir,
                    _rayLength,
                    point, 
                    _transparentWallPointHitRadius))
				{
					_intersectState = IntersectState::TransparentWallPoint;
					_editingWallIndex = static_cast<int>(i);
					_editingPointIndex = static_cast<int>(j);
					break;
				}
			}
		}

        switch (_intersectState)
        {
        case TerrainCollisionMaker::IntersectState::Terrain:
            // 地形と当たっていて左クリックが押されたらポイント追加
            if (_INPUT_RELEASED("LeftClick"))
            {
				// ポイント追加状態に移行
                ChangeState(State::CreatePoint);
            }
            break;
        case TerrainCollisionMaker::IntersectState::TransparentWallPoint:
			// 透明壁のポイントと当たっていて左クリックが押されたら編集状態に移行
            if (_INPUT_RELEASED("LeftClick"))
            {
                ChangeState(State::EditPoint);
            }
            break;
        }

		return;
	}
    else if (_state == State::CreatePoint)
    {
    }
    else if (_state == State::EditPoint)
    {
		Vector3& point = terrain->GetTransparentWalls()[_editingWallIndex].points[_editingPointIndex];
        DirectX::XMFLOAT4X4 transform{};
		DirectX::XMStoreFloat4x4(&transform, 
            DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&point)));
        if (Debug::Guizmo(
            view,
            projection,
            &transform))
        {
			// 単位を考慮した行列から位置、回転、スケールを取得
			DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&transform);
			DirectX::XMVECTOR S, R, T;
			DirectX::XMMatrixDecompose(&S, &R, &T, M);
			point.x = DirectX::XMVectorGetX(T);
			point.y = DirectX::XMVectorGetY(T);
			point.z = DirectX::XMVectorGetZ(T);
        }

        if (!ImGuizmo::IsOver() && !ImGuizmo::IsUsing())
        {
            if (_INPUT_RELEASED("LeftClick"))
            {
                // 編集状態を終了
                ChangeState(State::None);
            }
        }
        else
        {
            // 右クリックでポイントを削除
            if (_INPUT_RELEASED("RightClick"))
            {
                // 削除状態へ遷移
                ChangeState(State::RemovePoint);
            }
        }
    }
    else if (_state == State::RemovePoint)
    {
    }
}
// GUI描画
void TerrainCollisionMaker::DrawGui()
{
	ImGui::Checkbox(u8"透明壁編集", &_isEditing);
	ImGui::DragFloat(u8"透明壁ポイントの描画半径", &_transparentWallPointRadius, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat(u8"透明壁ポイントのヒット半径", &_transparentWallPointHitRadius, 0.01f, 0.0f, 1.0f);
	ImGui::ColorEdit4(u8"透明壁ポイントの色", &_transparentWallPointColor.x);
	ImGui::ColorEdit4(u8"透明壁ポイントの選択色", &_transparentWallPointSelectColor.x);
    ImGui::Separator();

    auto terrain = _terrainController.lock()->GetTerrain().lock();
    if (!terrain)
        return;

    if (_state == State::CreatePoint)
    {
        auto& walls = terrain->GetTransparentWalls();
        ImVec2 pos{};
        pos.x = _guiPosition.x;
        pos.y = _guiPosition.y;
        ImGui::SetNextWindowPos(pos);
        if (ImGui::Begin(u8"頂点の追加先"))
        {
            for (size_t i = 0; i < walls.size(); i++)
            {
				if (ImGui::Button((u8"壁：" + std::to_string(i)).c_str()))
				{
                    _editingWallIndex = static_cast<int>(i);
                    // ポイント追加
                    walls[_editingWallIndex].points.push_back(_intersectionWorldPoint);
                    _editingPointIndex = static_cast<int>(walls[_editingWallIndex].points.size() - 1);
                    // ポイント編集状態に移行
                    ChangeState(State::EditPoint);
				}
            }
            ImGui::Separator();

            if (ImGui::Button(u8"新規作成"))
            {
                // 新規作成ボタンが押されたら新しい透明壁を追加
                walls.push_back(Terrain::TransparentWall{});
                _editingWallIndex = static_cast<int>(walls.size() - 1);
                // ポイント追加
                walls[_editingWallIndex].points.push_back(_intersectionWorldPoint);
                _editingPointIndex = static_cast<int>(walls[_editingWallIndex].points.size() - 1);
                // ポイント編集状態に移行
                ChangeState(State::EditPoint);
            }
            ImGui::Separator();
            if (ImGui::Button(u8"キャンセル"))
                // 編集状態を終了
                ChangeState(State::None);
        }
        ImGui::End();
    }
    if (_state == State::RemovePoint)
    {
        if (ImGui::Begin(u8"頂点の削除"))
        {
            if (ImGui::Button(u8"削除"))
            {
                terrain->GetTransparentWalls()[_editingWallIndex].points.erase(
                    terrain->GetTransparentWalls()[_editingWallIndex].points.begin() + _editingPointIndex);
                // 壁に含まれるポイントがなくなった場合は壁自体を削除
                if (terrain->GetTransparentWalls()[_editingWallIndex].points.empty())
                {
                    terrain->GetTransparentWalls().erase(
                        terrain->GetTransparentWalls().begin() + _editingWallIndex);
                }
                // 編集状態を終了
                ChangeState(State::None);
            }
            ImGui::Separator();
            if (ImGui::Button(u8"キャンセル"))
                // 編集状態を終了
                ChangeState(State::None);
        }
        ImGui::End();
    }
}
// デバッグ描画
void TerrainCollisionMaker::DebugRender(const RenderContext& rc)
{
    // 地形コントローラーが取得できていない場合は何もしない
    if (!_terrainController.lock())
        return;
	// 透明壁に含まれるポイントを描画
	auto terrain = _terrainController.lock()->GetTerrain().lock();
	if (!terrain)
		return;

    switch (_state)
    {
    case TerrainCollisionMaker::State::None:
        switch (_intersectState)
        {
        case TerrainCollisionMaker::IntersectState::None:
            break;
        case TerrainCollisionMaker::IntersectState::Terrain:
            Debug::Renderer::DrawSphere(_intersectionWorldPoint, _transparentWallPointRadius, _transparentWallPointSelectColor);
            break;
        case TerrainCollisionMaker::IntersectState::TransparentWallPoint:
            break;
        default:
            break;
        }
        break;
    case TerrainCollisionMaker::State::CreatePoint:
        break;
    case TerrainCollisionMaker::State::EditPoint:
        break;
    case TerrainCollisionMaker::State::RemovePoint:
        break;
    default:
        break;
    }

	int wallIndex = 0;
	for (const auto& wall : terrain->GetTransparentWalls())
	{
        int pointIndex = 0;
		for (const auto& point : wall.points)
		{
            if (wallIndex == _editingWallIndex && pointIndex == _editingPointIndex)
                Debug::Renderer::DrawSphere(point, _transparentWallPointRadius, _transparentWallPointSelectColor);
            else
                Debug::Renderer::DrawSphere(point, _transparentWallPointRadius, _transparentWallPointColor);

            pointIndex++;
		}
        wallIndex++;
	}
}
// 状態変更
void TerrainCollisionMaker::ChangeState(State newState)
{
    switch (newState)
    {
    case TerrainCollisionMaker::State::None:
        // 選択を解除
        _editingWallIndex = -1;
        _editingPointIndex = -1;
        break;
    case TerrainCollisionMaker::State::CreatePoint:
		// クリックしたスクリーン座標を保存
        _guiPosition.x = ImGui::GetMousePos().x;
        _guiPosition.y = ImGui::GetMousePos().y;
        break;
    case TerrainCollisionMaker::State::EditPoint:
        break;
    case TerrainCollisionMaker::State::RemovePoint:
        break;
    default:
        break;
    }
	_state = newState;
}
