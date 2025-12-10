#include "TransparentWallCollider.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Collision/CollisionMath.h"

#include <imgui.h>
#include <ImGuizmo.h>

// 生成時処理
void TransparentWallCollider::OnCreate()
{
	// データ読み込み
	LoadFromFile();
	// Undo/Redoコールバック設定
	_transparentWall.SetOnUndoCallback([this]() {
		ChangeState(EditState::None);
		});
	_transparentWall.SetOnRedoCallback([this]() {
		ChangeState(EditState::None);
		});

	MeshCollider::OnCreate();
}

// 更新処理
void TransparentWallCollider::Update(float elapsedTime)
{
	MeshCollider::Update(elapsedTime);
	if (_isEditing)
		EditingUpdate(elapsedTime);
}

// デバッグ描画処理
void TransparentWallCollider::DebugRender(const RenderContext& rc)
{
	MeshCollider::DebugRender(rc);
	if (_debugRender)
	{
		for (const auto& wall : _transparentWall.GetWalls())
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
	if (_isEditing)
		EditingDebugRender(rc);
}

// GUI描画
void TransparentWallCollider::DrawGui()
{
	MeshCollider::DrawGui();
	ImGui::Separator();
	_transparentWall.DrawGui();
	ImGui::Checkbox(u8"編集モード", &_isEditing);
	ImGui::Checkbox(u8"デバッグ表示", &_debugRender);
	ImGui::Checkbox(u8"頂点スナップ", &_snapToGround);

	switch (_state)
	{
	case EditState::None:       break;
	case EditState::CreatePoint:DrawGuiCreatePoint();   break;
	case EditState::MovePoint:  break;
	case EditState::EditPoint:  DrawGuiEditPoint();     break;
	default:
		break;
	}
}

// コリジョンメッシュの再計算
MeshCollider::CollisionMesh TransparentWallCollider::RecalculateCollisionMesh(Model* model) const
{
	CollisionMesh collisionMesh;
	Vector3 volumeMin = Vector3::Max;
	Vector3 volumeMax = Vector3::Min;

	// 透明壁のデータから三角形を生成
	for (size_t i = 0; i < _transparentWall.GetWalls().size(); i++)
	{
		auto& wall = _transparentWall.GetWalls()[i];
		size_t pointCount = wall.vertices.size();
		if (pointCount <= 1)
			continue;
		Vector3 heightOffset = Vector3(0.0f, wall.height, 0.0f);
		for (size_t i = 0; i < pointCount - 1; i++)
		{
			const Vector3& p1 = wall.vertices[i];
			const Vector3& p2 = wall.vertices[i + 1];
			const Vector3& p3 = p1 + heightOffset;
			const Vector3& p4 = p2 + heightOffset;

			// 法線ベクトルを算出
			Vector3 normal = (p2 - p1).Cross(p3 - p1).Normalize();
			// 三角形データを格納
			CollisionMesh::Triangle& triangle0 = collisionMesh.triangles.emplace_back();
			triangle0.positions[0] = p1;
			triangle0.positions[1] = p2;
			triangle0.positions[2] = p3;
			triangle0.normal = normal;
			CollisionMesh::Triangle& triangle1 = collisionMesh.triangles.emplace_back();
			triangle1.positions[0] = p2;
			triangle1.positions[1] = p4;
			triangle1.positions[2] = p3;
			triangle1.normal = normal;

			// モデル全体のAABBを計測
			volumeMin = volumeMin.Minimum(p1);
			volumeMax = volumeMax.Maximum(p1);
			volumeMin = volumeMin.Minimum(p2);
			volumeMax = volumeMax.Maximum(p2);
			volumeMin = volumeMin.Minimum(p3);
			volumeMax = volumeMax.Maximum(p3);
			volumeMin = volumeMin.Minimum(p4);
			volumeMax = volumeMax.Maximum(p4);
		}
	}

	// AABBの構築
	BuildCollisionMeshAABB(collisionMesh, volumeMin, volumeMax);

	return collisionMesh;
}

#pragma region 入出力
// ファイル読み込み
bool TransparentWallCollider::LoadFromFile()
{
	std::string filePath = this->GetDirectory();
	filePath += this->GetName();
	filePath += ".json";

	// 透明壁の読み込み
	return _transparentWall.LoadFromFile(filePath);
}

// ファイル保存
bool TransparentWallCollider::SaveToFile()
{
	std::string filePath = this->GetDirectory();
	filePath += this->GetName();
	filePath += ".json";

	return _transparentWall.SaveToFile(filePath);
}
#pragma endregion

// 編集処理
void TransparentWallCollider::EditingUpdate(float elapsedTime)
{
	// 入力状態の更新
	_inputState = EditInputState::NoneInput;
	if (_INPUT_RELEASED("LeftClick"))
	{
		// ダブルクリック判定
		if (_leftClickTime < _doubleClickThreshold)
			_inputState = EditInputState::LeftDoubleClick;
		else
			_inputState = EditInputState::LeftClick;
		_leftClickTime = 0.0f;
	}
	else if (_INPUT_RELEASED("RightClick"))
	{
		_inputState = EditInputState::RightClick;
	}
	_leftClickTime += elapsedTime;


	// アクターのギズモ使用フラグを常にオフ
	GetActor()->SetIsUsingGuizmo(false);
	_intersectState = IntersectState::NoneIntersect;

	if (_enableMouseCollision)
	{
		// マウスのスクリーン座標取得
		Vector3 mousePos{};
		mousePos.x = Input::Instance().GetMouseInput()->GetCurrentCursorPosX();
		mousePos.y = Input::Instance().GetMouseInput()->GetCurrentCursorPosY();
		float screenWidth = Graphics::Instance().GetScreenWidth();
		float screenHeight = Graphics::Instance().GetScreenHeight();
		auto& view = GetActor()->GetScene()->GetMainCamera()->GetView();
		auto& projection = GetActor()->GetScene()->GetMainCamera()->GetProjection();

		// マウス座標から地形との当たり判定
		mousePos.z = 0.0f;
		Vector3 rayStart = mousePos.Unproject(screenWidth, screenHeight, view, projection);
		mousePos.z = 1.0f;
		Vector3 rayEnd = mousePos.Unproject(screenWidth, screenHeight, view, projection);
		Vector3 rayDir = (rayEnd - rayStart).Normalize();
		float distance = _rayLength;
		// レイキャストで交差点を取得
		auto actors = GetActor()->GetScene()->GetCollisionManager().RayCast(rayStart, rayDir, distance);
		if (actors.size() > 0)
		{
			// このオブジェクト以外と交差していたら交差点を保存
			for (auto& result : actors)
			{
				if (result.hitActor != this->GetActor().get())
				{
					_intersectionWorldPoint = result.hitPosition;
					_intersectState |= IntersectState::Stage;
					break;
				}
			}
		}

		// 透明壁のポイントとの交差判定
		for (size_t i = 0; i < _transparentWall.GetWalls().size(); i++)
		{
			const auto& wall = _transparentWall.GetWalls()[i];
			for (size_t j = 0; j < wall.vertices.size(); j++)
			{
				const Vector3& point = wall.vertices[j];
				// 透明壁のポイントとの交差判定
				if (Collision3D::IntersectRayVsSphere(
					rayStart,
					rayDir,
					_rayLength,
					point,
					_vertexCollisionRadius))
				{
					_intersectState |= IntersectState::WallPoint;
					_editingWallIndex = static_cast<int>(i);
					_editingPointIndex = static_cast<int>(j);
					break;
				}
			}
		}
	}

	switch (_state)
	{
	case EditState::None:       UpdateNone(elapsedTime);		break;
	case EditState::CreatePoint:UpdateCreatePoint(elapsedTime); break;
	case EditState::MovePoint:  UpdateMovePoint(elapsedTime);   break;
	case EditState::EditPoint:  UpdateEditPoint(elapsedTime);   break;
	}
}

// 状態変更
void TransparentWallCollider::ChangeState(EditState newState)
{
	switch (newState)
	{
	case EditState::None:
		// 選択を解除
		_editingWallIndex = -1;
		_editingPointIndex = -1;
		_enableMouseCollision = true;
		break;
	case EditState::CreatePoint:
		// クリックしたスクリーン座標を保存
		_guiPosition.x = ImGui::GetMousePos().x;
		_guiPosition.y = ImGui::GetMousePos().y;
		_enableMouseCollision = false;
		break;
	case EditState::MovePoint:
		if (_editingWallIndex != -1 && _editingPointIndex != -1)
			_editingVertex = _transparentWall.GetWalls()[_editingWallIndex].vertices[_editingPointIndex];
		else
		{
			ChangeState(EditState::None);
			return;
		}
		_enableMouseCollision = false;
		break;
	case EditState::EditPoint:
		_enableMouseCollision = true;
		break;
	}
	_state = newState;
}

// 編集中のデバッグ描画
void TransparentWallCollider::EditingDebugRender(const RenderContext& rc)
{
	switch (_state)
	{
	case EditState::None:
		if (_intersectState & IntersectState::WallPoint)
		{
			if (_editingWallIndex != -1 && _editingPointIndex != -1)
			{
				Debug::Renderer::DrawSphere(
					_transparentWall.GetWalls()[_editingWallIndex].vertices[_editingPointIndex],
					_vertexRadius,
					_selectedVertexColor);
			}
		}
		else if (_intersectState & IntersectState::Stage)
		{
			Debug::Renderer::DrawSphere(_intersectionWorldPoint, _vertexRadius, _selectedVertexColor);
		}
		break;
	case EditState::CreatePoint:
		break;
	case EditState::MovePoint:
		break;
	case EditState::EditPoint:
		break;
	default:
		break;
	}

	// 透明壁に含まれる頂点を描画
	int wallIndex = 0;
	for (const auto& wall : _transparentWall.GetWalls())
	{
		int pointIndex = 0;
		for (const auto& vertex : wall.vertices)
		{
			// 番号表示
			GetActor()->GetScene()->GetTextRenderer().Draw3D(
				FontType::MSGothic,
				std::to_string(pointIndex).c_str(),
				vertex,
				Vector4::White,
				0.0f,
				Vector2::Zero,
				Vector2(0.5f, 0.5f));
			if (wallIndex == _editingWallIndex && pointIndex == _editingPointIndex)
			{
				if (_state == EditState::MovePoint)
				{
					pointIndex++;
					continue;
				}

				Debug::Renderer::DrawSphere(vertex, _vertexRadius, _selectedVertexColor);
			}
			else
			{
				Debug::Renderer::DrawSphere(vertex, _vertexRadius, _vertexColor);
			}


			pointIndex++;
		}
		wallIndex++;
	}
}

#pragma region 各状態の更新処理
void TransparentWallCollider::UpdateNone(float elapsedTime)
{
	if (_intersectState & IntersectState::WallPoint)
	{
		// 透明壁のポイントと当たっていて左クリックされたら移動状態に移行
		if (_inputState == EditInputState::LeftClick)
		{
			ChangeState(EditState::MovePoint);
		}
		// 透明壁のポイントと当たっていて右クリックが押されたら編集状態に移行
		else if (_inputState == EditInputState::RightClick)
		{
			ChangeState(EditState::EditPoint);
		}
	}
	else if (_intersectState & IntersectState::Stage)
	{
		// 地形と当たっていて左ダブルクリックされたらポイント追加
		if (_inputState == EditInputState::LeftDoubleClick)
		{
			// ポイント追加状態に移行
			ChangeState(EditState::CreatePoint);
		}
	}
}

void TransparentWallCollider::UpdateCreatePoint(float elapsedTime)
{
	//	ウィンドウにフォーカス中の場合は処理しない
	if (ImGui::IsAnyItemActive() || ImGui::IsAnyItemFocused() || ImGui::IsAnyItemHovered())
	{
		return;
	}
	if (_inputState == EditInputState::LeftClick)
	{
		ChangeState(EditState::None);
		return;
	}
}

void TransparentWallCollider::UpdateMovePoint(float elapsedTime)
{
	auto& walls = _transparentWall.GetWalls();
	const DirectX::XMFLOAT4X4& view = GetActor()->GetScene()->GetMainCamera()->GetView();
	const DirectX::XMFLOAT4X4& projection = GetActor()->GetScene()->GetMainCamera()->GetProjection();

	if (_editingWallIndex >= walls.size())
		return;

	// ギズモでポイントを移動
	Vector3 worldPosition = _editingVertex;

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
		DirectX::XMStoreFloat3(&worldPosition, T);
		// 地面にスナップ
		if (_snapToGround)
		{
			// 地形との交差判定
			// レイキャストで交差点を取得
			auto actors = GetActor()->GetScene()->GetCollisionManager().RayCast(
				worldPosition + Vector3(Vector3::Up) * _rayLength / 2.0f,
				Vector3::Down,
				_rayLength);
			if (actors.size() > 0)
			{
				// このオブジェクト以外と交差しているか
				for (auto& result : actors)
				{
					if (result.hitActor != this->GetActor().get())
					{
						worldPosition.y = result.hitPosition.y;
						break;
					}
				}
			}
		}
		_editingVertex = worldPosition;
	}

	// ポイント移動確定
	if (!ImGuizmo::IsUsing())
	{
		if (walls[_editingWallIndex].vertices[_editingPointIndex] != _editingVertex)
		{
			_transparentWall.MoveVertex(
				_editingWallIndex,
				_editingPointIndex,
				_editingVertex);
		}
	}

	if (!ImGuizmo::IsOver() && !ImGuizmo::IsUsing())
	{
		if (_inputState == EditInputState::LeftClick)
		{
			// 移動状態を終了
			ChangeState(EditState::None);
			return;
		}
	}
	// 右クリックでポイントを編集
	if (_inputState == EditInputState::RightClick)
	{
		// 編集状態へ遷移
		ChangeState(EditState::EditPoint);
	}
}

void TransparentWallCollider::UpdateEditPoint(float elapsedTime)
{
}
#pragma endregion

#pragma region 各状態のGUI描画
void TransparentWallCollider::DrawGuiCreatePoint()
{
#ifdef USE_IMGUI
	auto& walls = _transparentWall.GetWalls();

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
				_editingPointIndex = _transparentWall.AddVertexToFront(_editingWallIndex, _intersectionWorldPoint);
				if (_editingPointIndex == -1)
					// ポイント追加に失敗した場合は状態を終了
					ChangeState(EditState::None);
				else
					// ポイント移動状態に移行
					ChangeState(EditState::MovePoint);
			}
			ImGui::SameLine();
			if (ImGui::Button((std::to_string(i) + u8"末尾に追加").c_str()))
			{
				_editingWallIndex = static_cast<int>(i);
				// ポイント追加
				_editingPointIndex = _transparentWall.AddVertexToBack(_editingWallIndex, _intersectionWorldPoint);
				if (_editingPointIndex == -1)
					// ポイント追加に失敗した場合は状態を終了
					ChangeState(EditState::None);
				else
					// ポイント移動状態に移行
					ChangeState(EditState::MovePoint);
			}
		}
		ImGui::Separator();

		if (ImGui::Button(u8"新規作成"))
		{
			// 透明壁を追加
			_editingWallIndex = _transparentWall.AddWall();
			// ポイント追加
			_editingPointIndex = _transparentWall.AddVertexToBack(_editingWallIndex, _intersectionWorldPoint);
			if (_editingPointIndex == -1)
				// ポイント追加に失敗した場合は状態を終了
				ChangeState(EditState::None);
			else
				// ポイント移動状態に移行
				ChangeState(EditState::MovePoint);
		}
		ImGui::Separator();
		if (ImGui::Button(u8"キャンセル"))
			// 移動状態を終了
			ChangeState(EditState::None);
	}
	ImGui::End();
#endif
}

void TransparentWallCollider::DrawGuiEditPoint()
{
#ifdef USE_IMGUI
	auto& walls = _transparentWall.GetWalls();
	const auto& vertices = _transparentWall.GetVertices(_editingWallIndex);

	if (ImGui::Begin(u8"頂点編集"))
	{
		if (ImGui::Button(u8"0番目の頂点座標へ移動"))
		{
			_transparentWall.MoveVertex(
				_editingWallIndex,
				_editingPointIndex,
				vertices[0]);
			// ポイント移動状態に移行
			ChangeState(EditState::MovePoint);
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"最後の頂点座標へ移動"))
		{
			_transparentWall.MoveVertex(
				_editingWallIndex,
				_editingPointIndex,
				vertices[vertices.size() - 1]);
			// ポイント移動状態に移行
			ChangeState(EditState::MovePoint);
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
			_editingPointIndex = _transparentWall.AddVertexToIndex(_editingWallIndex, _editingPointIndex, pos);
			if (_editingPointIndex == -1)
				// ポイント追加に失敗した場合は状態を終了
				ChangeState(EditState::None);
			else
				// ポイント移動状態に移行
				ChangeState(EditState::MovePoint);
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
			_editingPointIndex = _transparentWall.AddVertexToIndex(_editingWallIndex, _editingPointIndex + 1, pos);
			if (_editingPointIndex == -1)
				// ポイント追加に失敗した場合は状態を終了
				ChangeState(EditState::None);
			else
				// ポイント移動状態に移行
				ChangeState(EditState::MovePoint);
		}
		ImGui::Separator();
		if (ImGui::Button(u8"削除"))
		{
			_transparentWall.RemoveVertex(_editingWallIndex, _editingPointIndex);
			// 編集状態を終了
			ChangeState(EditState::None);
		}
		ImGui::Separator();
		if (ImGui::Button(u8"キャンセル"))
			// 編集状態を終了
			ChangeState(EditState::None);
	}
	ImGui::End();
#endif
}
#pragma endregion
