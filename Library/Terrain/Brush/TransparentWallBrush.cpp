#include "TransparentWallBrush.h"

#include "../../Library/Input/Input.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Scene/Scene.h"
#include "../../Library/Collision/CollisionMath.h"

#include <imgui.h>
#include <ImGuizmo.h>

TransparentWallBrush::TransparentWallBrush(TerrainDeformer* deformer) :
	TerrainDeformerBrush(deformer)
{
}

void TransparentWallBrush::Update(std::vector<std::shared_ptr<TerrainController>>& terrainControllers,
    float elapsedTime,
    Vector3* intersectWorldPosition)
{
    if (terrainControllers.empty())
		return;

    // 地形の取得
    _terrain = terrainControllers[0]->GetTerrain().lock();
    if (!_terrain)
        return;

    _brushRadius = _vertexRadius;
    _drawDebugBrush = false;

    // アクターのギズモ使用フラグを常にオフ
    _deformer->GetActor()->SetIsUsingGuizmo(false);

    switch (_state)
    {
    case State::None:       UpdateNone(_terrain, elapsedTime);   break;
    case State::CreatePoint:UpdateCreatePoint(_terrain, elapsedTime);   break;
    case State::MovePoint:  UpdateMovePoint(_terrain, elapsedTime);   break;
    case State::EditPoint:  UpdateEditPoint(_terrain, elapsedTime);   break;
    default:
        break;
    }
}
// GUI描画
void TransparentWallBrush::DrawGui()
{
	if (!_terrain)
		return;

    ImGui::Checkbox(u8"頂点をスナップするか", &_snapToGround);
    if (_editingWallIndex != -1)
    {
        auto& walls = _terrain->GetTransparentWall()->GetWalls();
		auto& wall = walls[_editingWallIndex];
        wall.DrawGui();
        ImGui::Separator();
		if (ImGui::Button(u8"選択中の透明壁を削除"))
		{
			// 選択中の透明壁を削除
            _terrain->GetTransparentWall()->RemoveWall(_editingWallIndex);
			// 編集状態を解除
			ChangeState(State::None);
		}
	}
    ImGui::Separator();
    switch (_state)
    {
    case State::None:       break;
    case State::CreatePoint:DrawGuiCreatePoint(_terrain);   break;
    case State::MovePoint:  break;
    case State::EditPoint:  DrawGuiEditPoint(_terrain);     break;
    default:
        break;
    }
    // デバッグ描画
    DebugRender(_terrain);
}

void TransparentWallBrush::ChangeState(State newState)
{
    switch (newState)
    {
    case State::None:
        // 選択を解除
        _editingWallIndex = -1;
        _editingPointIndex = -1;
        break;
    case State::CreatePoint:
        // クリックしたスクリーン座標を保存
        _guiPosition.x = ImGui::GetMousePos().x;
        _guiPosition.y = ImGui::GetMousePos().y;
        break;
    case State::MovePoint:
        break;
    case State::EditPoint:
        break;
    default:
        break;
    }
    _state = newState;
}

#pragma region 各状態の更新処理
void TransparentWallBrush::UpdateNone(
    std::shared_ptr<Terrain> terrain,
    float elapsedTime)
{
    auto& walls = terrain->GetTransparentWall()->GetWalls();
    float screenWidth = Graphics::Instance().GetScreenWidth();
    float screenHeight = Graphics::Instance().GetScreenHeight();
    const DirectX::XMFLOAT4X4& view = _deformer->GetActor()->GetScene()->GetMainCamera()->GetView();
    const DirectX::XMFLOAT4X4& projection = _deformer->GetActor()->GetScene()->GetMainCamera()->GetProjection();
    const DirectX::XMFLOAT4X4& world = _deformer->GetActor()->GetTransform().GetMatrix();

    _clickScreenPoint.x = Input::Instance().GetMouseInput()->GetCurrentCursorPosX();
    _clickScreenPoint.y = Input::Instance().GetMouseInput()->GetCurrentCursorPosY();
    _intersectState = IntersectState::None;
    // マウス座標から地形との当たり判定
    Vector3 rayStart = Vector3::Unproject(Vector3(_clickScreenPoint.x, _clickScreenPoint.y, 0.0f), screenWidth, screenHeight, view, projection);
    Vector3 rayEnd = Vector3::Unproject(Vector3(_clickScreenPoint.x, _clickScreenPoint.y, 1.0f), screenWidth, screenHeight, view, projection);
    Vector3 rayDir = (rayEnd - rayStart).Normalize();
    // 地形との交差判定
    if (terrain->Raycast(world,
        rayStart, rayDir, _rayLength,
        &_intersectionWorldPoint))
        _intersectState = IntersectState::Terrain;

    // 透明壁のポイントとの交差判定
    for (size_t i = 0; i < walls.size(); i++)
    {
        const auto& wall = walls[i];
        for (size_t j = 0; j < wall.vertices.size(); j++)
        {
            const Vector3& point = wall.vertices[j].TransformCoord(world);
            // 透明壁のポイントとの交差判定
            if (Collision3D::IntersectRayVsSphere(
                rayStart,
                rayDir,
                _rayLength,
                point,
                _vertexCollisionRadius))
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
    case IntersectState::Terrain:
        // 地形と当たっていて左クリックが押されたらポイント追加
        if (_INPUT_RELEASED("LeftClick"))
        {
            // ポイント追加状態に移行
            ChangeState(State::CreatePoint);
        }
        break;
    case IntersectState::TransparentWallPoint:
        // 透明壁のポイントと当たっていて左クリックが押されたら移動状態に移行
        if (_INPUT_RELEASED("LeftClick"))
        {
            ChangeState(State::MovePoint);
        }
		// 透明壁のポイントと当たっていて右クリックが押されたら編集状態に移行
        if (_INPUT_RELEASED("RightClick"))
        {
            ChangeState(State::EditPoint);
        }
        break;
    }
}

void TransparentWallBrush::UpdateCreatePoint(std::shared_ptr<Terrain> terrain, float elapsedTime)
{
    if (_INPUT_RELEASED("LeftClick"))
    {
        ChangeState(State::None);
        return;
    }
}

void TransparentWallBrush::UpdateMovePoint(std::shared_ptr<Terrain> terrain, float elapsedTime)
{
    auto& walls = terrain->GetTransparentWall()->GetWalls();
    const DirectX::XMFLOAT4X4& world = _deformer->GetActor()->GetTransform().GetMatrix();
    const DirectX::XMFLOAT4X4& invWorld = _deformer->GetActor()->GetTransform().GetMatrixInverse();
    const DirectX::XMFLOAT4X4& view = _deformer->GetActor()->GetScene()->GetMainCamera()->GetView();
    const DirectX::XMFLOAT4X4& projection = _deformer->GetActor()->GetScene()->GetMainCamera()->GetProjection();

    // ギズモでポイントを移動
    Vector3& point = walls[_editingWallIndex].vertices[_editingPointIndex];
    Vector3 worldPosition = point.TransformCoord(world);

    DirectX::XMFLOAT4X4 transform{};
    DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&worldPosition)));
    if (Debug::Guizmo(
        view,
        projection,
        &transform))
    {
        // 単位を考慮した行列から位置、回転、スケールを取得
        DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&transform);
        DirectX::XMVECTOR S, R, T;
        DirectX::XMMatrixDecompose(&S, &R, &T, M);
        worldPosition.x = DirectX::XMVectorGetX(T);
        worldPosition.y = DirectX::XMVectorGetY(T);
        worldPosition.z = DirectX::XMVectorGetZ(T);
        point = worldPosition.TransformCoord(invWorld);
    }

    // 地面にスナップ
    if (_snapToGround)
    {
        // 地形との交差判定
        Vector3 hitPosition{};
        if (terrain->Raycast(
            world,
            worldPosition + Vector3(Vector3::Up) * _rayLength / 2.0f,
            Vector3::Down,
            _rayLength,
            &hitPosition))
        {
            worldPosition.y = hitPosition.y;
            // 交差点をローカル空間に変換
            point.y = hitPosition.TransformCoord(invWorld).y;
        }
    }

    if (!ImGuizmo::IsOver() && !ImGuizmo::IsUsing())
    {
        if (_INPUT_RELEASED("LeftClick"))
        {
            // 移動状態を終了
            ChangeState(State::None);
            return;
        }
    }
    // 右クリックでポイントを編集
    if (_INPUT_RELEASED("RightClick"))
    {
        // 編集状態へ遷移
        ChangeState(State::EditPoint);
    }
}

void TransparentWallBrush::UpdateEditPoint(std::shared_ptr<Terrain> terrain, float elapsedTime)
{
}
#pragma endregion

#pragma region 各状態のGUI描画
void TransparentWallBrush::DrawGuiCreatePoint(std::shared_ptr<Terrain> terrain)
{
#ifdef USE_IMGUI
    auto& walls = terrain->GetTransparentWall()->GetWalls();
    const DirectX::XMFLOAT4X4& invWorld = _deformer->GetActor()->GetTransform().GetMatrixInverse();
    Vector3 intersectionLocalPoint = _intersectionWorldPoint.TransformCoord(invWorld);

    ImVec2 pos{};
    pos.x = _guiPosition.x;
    pos.y = _guiPosition.y;
    ImGui::SetNextWindowPos(pos);
    if (ImGui::Begin(u8"頂点の追加先"))
    {
        for (size_t i = 0; i < walls.size(); i++)
        {
            if (ImGui::Button((std::to_string(i) + u8"先頭に追加").c_str()))
            {
                _editingWallIndex = static_cast<int>(i);
                // ポイント追加
                _editingPointIndex = terrain->GetTransparentWall()->AddVertexToFront(_editingWallIndex, intersectionLocalPoint);
				if (_editingPointIndex == -1)
                    // ポイント追加に失敗した場合は状態を終了
                    ChangeState(State::None);
                else
                    // ポイント移動状態に移行
                    ChangeState(State::MovePoint);
            }
            ImGui::SameLine();
            if (ImGui::Button((std::to_string(i) + u8"末尾に追加").c_str()))
            {
                _editingWallIndex = static_cast<int>(i);
                // ポイント追加
                _editingPointIndex = terrain->GetTransparentWall()->AddVertexToBack(_editingWallIndex, intersectionLocalPoint);
                if (_editingPointIndex == -1)
                    // ポイント追加に失敗した場合は状態を終了
                    ChangeState(State::None);
                else
                    // ポイント移動状態に移行
                    ChangeState(State::MovePoint);
            }
        }
        ImGui::Separator();

        if (ImGui::Button(u8"新規作成"))
        {
            // 透明壁を追加
            _editingWallIndex = terrain->GetTransparentWall()->AddWall();
            // ポイント追加
            _editingPointIndex = terrain->GetTransparentWall()->AddVertexToBack(_editingWallIndex, intersectionLocalPoint);
            if (_editingPointIndex == -1)
                // ポイント追加に失敗した場合は状態を終了
                ChangeState(State::None);
            else
                // ポイント移動状態に移行
                ChangeState(State::MovePoint);
        }
        ImGui::Separator();
        if (ImGui::Button(u8"キャンセル"))
            // 移動状態を終了
            ChangeState(State::None);
    }
    ImGui::End();
#endif
}
void TransparentWallBrush::DrawGuiEditPoint(std::shared_ptr<Terrain> terrain)
{
#ifdef USE_IMGUI
    auto& walls = terrain->GetTransparentWall()->GetWalls();
	auto& vertices = terrain->GetTransparentWall()->GetVertices(_editingWallIndex);
    if (ImGui::Begin(u8"頂点編集"))
    {
        if (ImGui::Button(u8"0番目の頂点座標へ移動"))
        {
            vertices[_editingPointIndex] = vertices[0];
            // ポイント移動状態に移行
            ChangeState(State::MovePoint);
        }
		ImGui::SameLine();
		if (ImGui::Button(u8"最後の頂点座標へ移動"))
		{
			vertices[_editingPointIndex] = vertices[vertices.size() - 1];
            // ポイント移動状態に移行
            ChangeState(State::MovePoint);
		}
        ImGui::Separator();
        if (ImGui::Button(u8"一つ前に頂点追加"))
        {
            Vector3 pos = vertices[_editingPointIndex];
            if (_editingPointIndex > 0)
            {
                // 編集中のポイントと前のポイントの間に交差点を挿入
                int prevIndex = _editingPointIndex - 1;
                pos = (pos + vertices[prevIndex]) / 2.0f;
            }
            // ポイント追加
            _editingPointIndex = terrain->GetTransparentWall()->AddVertexToIndex(_editingWallIndex, _editingPointIndex, pos);
            if (_editingPointIndex == -1)
                // ポイント追加に失敗した場合は状態を終了
                ChangeState(State::None);
            else
                // ポイント移動状態に移行
                ChangeState(State::MovePoint);
        }
        ImGui::SameLine();
        if (ImGui::Button(u8"一つ先に頂点追加"))
        {
            Vector3 pos = vertices[_editingPointIndex];
            if (_editingPointIndex < vertices.size() - 1)
            {
                // 編集中のポイントと次のポイントの間に交差点を挿入
                int nextIndex = _editingPointIndex + 1;
                pos = (pos + vertices[nextIndex]) / 2.0f;
            }
            // ポイント追加
            _editingPointIndex = terrain->GetTransparentWall()->AddVertexToIndex(_editingWallIndex, _editingPointIndex + 1, pos);
            if (_editingPointIndex == -1)
                // ポイント追加に失敗した場合は状態を終了
                ChangeState(State::None);
            else
                // ポイント移動状態に移行
                ChangeState(State::MovePoint);
        }
        ImGui::Separator();
        if (ImGui::Button(u8"削除"))
        {
			terrain->GetTransparentWall()->RemoveVertex(_editingWallIndex, _editingPointIndex);
            // 編集状態を終了
            ChangeState(State::None);
        }
        ImGui::Separator();
        if (ImGui::Button(u8"キャンセル"))
            // 編集状態を終了
            ChangeState(State::None);
    }
    ImGui::End();
#endif
}
#pragma endregion

// デバッグ描画
void TransparentWallBrush::DebugRender(std::shared_ptr<Terrain> terrain)
{
    auto& walls = terrain->GetTransparentWall()->GetWalls();
    const DirectX::XMFLOAT4X4& world = _deformer->GetActor()->GetTransform().GetMatrix();

    switch (_state)
    {
    case State::None:
        switch (_intersectState)
        {
        case IntersectState::None:
            break;
        case IntersectState::Terrain:
            Debug::Renderer::DrawSphere(_intersectionWorldPoint, _vertexRadius, _selectedVertexColor);
            break;
        case IntersectState::TransparentWallPoint:
            break;
        default:
            break;
        }
        break;
    case State::CreatePoint:
        break;
    case State::MovePoint:
        break;
    case State::EditPoint:
        break;
    default:
        break;
    }

	// 透明壁に含まれる頂点を描画
    int wallIndex = 0;
    for (const auto& wall : walls)
    {
        int pointIndex = 0;
        for (const auto& vertex : wall.vertices)
        {
			Vector3 worldPoint = vertex.TransformCoord(world);

            if (wallIndex == _editingWallIndex && pointIndex == _editingPointIndex)
                Debug::Renderer::DrawSphere(worldPoint, _vertexRadius, _selectedVertexColor);
			else
                Debug::Renderer::DrawSphere(worldPoint, _vertexRadius, _vertexColor);

            // 番号表示
            _deformer->GetActor()->GetScene()->GetTextRenderer().Draw3D(
                FontType::MSGothic,
                std::to_string(pointIndex).c_str(),
                worldPoint,
                Vector4::White,
                0.0f,
                Vector2::Zero,
                Vector2(0.5f, 0.5f));

            pointIndex++;
        }
        wallIndex++;
    }
}
