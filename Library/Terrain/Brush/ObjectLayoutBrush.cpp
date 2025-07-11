#include "ObjectLayoutBrush.h"

#include "../../Library/Input/Input.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Scene/Scene.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"
#include "../../Library/Collision/CollisionMath.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <Mygui.h>

ObjectLayoutBrush::ObjectLayoutBrush(TerrainDeformer* deformer) :
	TerrainDeformerBrush(deformer)
{
}
// 更新処理
void ObjectLayoutBrush::Update(std::shared_ptr<Terrain> terrain, float elapsedTime)
{
	_drawDebugBrush = false;
	// アクターのギズモ使用フラグを常にオフ
	_deformer->GetActor()->SetUseGuizmoFlag(false);
	switch (_state)
	{
	case State::None:          UpdateNone(terrain, elapsedTime);       break;
	case State::CreateObject:  UpdateCreateObject(terrain, elapsedTime); break;
	case State::MoveObject:    UpdateMoveObject(terrain, elapsedTime);   break;
	case State::EditObject:    UpdateEditObject(terrain, elapsedTime);   break;
	default:
		break;
	}
}
// GUI描画
void ObjectLayoutBrush::DrawGui(std::shared_ptr<Terrain> terrain)
{
	if (_selectingModel)
	{
		ImGui::Text(u8"選択中のモデル: %s", _selectingModelPath.c_str());
	}
	else
	{
		ImGui::Text(u8"選択中のモデル: なし");
	}
	ImGui::Separator();
	switch (_state)
	{
	case State::None:
		break;
	case State::CreateObject:
		ImGui::Text(u8"オブジェクトを配置");
		break;
	case State::MoveObject:
		ImGui::Text(u8"オブジェクトを移動");
		break;
	case State::EditObject:
		ImGui::Text(u8"オブジェクトを編集");
		break;
	default:
		break;
	}
	ImGui::Separator();
	ImGui::DragFloat3(u8"オブジェクトのスケール", &_objectScale.x, 0.01f, 0.01f, 10.0f);
	Vector3 deg = _objectAngle.ToDegrees();
	if (ImGui::DragFloat3(u8"オブジェクトの角度", &deg.x, 0.1f))
		_objectAngle = deg.ToRadians();
}
// 描画処理
void ObjectLayoutBrush::Render(std::shared_ptr<Terrain> terrain, const RenderContext& rc, ID3D11ShaderResourceView** srv, uint32_t startSlot, uint32_t numViews)
{
}
// 状態変更
void ObjectLayoutBrush::ChangeState(State newState)
{
	// 状態を変更
	_state = newState;
	// 状態に応じて処理を行う
	switch (_state)
	{
	case State::None:
		// 選択を解除
		break;
	case State::CreateObject:
		// クリックしたスクリーン座標を保存
		_guiPosition.x = ImGui::GetMousePos().x;
		_guiPosition.y = ImGui::GetMousePos().y;
		break;
	case State::MoveObject:
		break;
	case State::EditObject:
		break;
	default:
		break;
	}
}
#pragma region 各状態の更新処理
void ObjectLayoutBrush::UpdateNone(std::shared_ptr<Terrain> terrain, float elapsedTime)
{
	auto& walls = terrain->GetTransparentWall()->GetWalls();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();
	const DirectX::XMFLOAT4X4& view = _deformer->GetActor()->GetScene()->GetMainCamera()->GetView();
	const DirectX::XMFLOAT4X4& projection = _deformer->GetActor()->GetScene()->GetMainCamera()->GetProjection();
	const DirectX::XMFLOAT4X4& world = _deformer->GetActor()->GetTransform().GetMatrix();

	_clickScreenPoint.x = _INPUT_VALUE("MousePositionX");
	_clickScreenPoint.y = _INPUT_VALUE("MousePositionY");
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

	// 選択中のモデルの変更
	if (!_selectingModel && !_deformer->GetSelectedModelPath().empty())
	{
		_selectingModel = std::make_unique<Model>(Graphics::Instance().GetDevice(), _deformer->GetSelectedModelPath().c_str());
		_selectingModelPath = _deformer->GetSelectedModelPath();
	}
	else if (_deformer->GetSelectedModelPath() != _selectingModelPath)
	{
		_selectingModel = std::make_unique<Model>(Graphics::Instance().GetDevice(), _deformer->GetSelectedModelPath().c_str());
		_selectingModelPath = _deformer->GetSelectedModelPath();
	}

	// 選択中のモデルのデバッグ描画
	if (_state == State::None && _intersectState == IntersectState::Terrain)
	{
		if (_selectingModel)
		{
			DirectX::XMFLOAT4X4 world{};
			DirectX::XMStoreFloat4x4(&world,
				DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&_objectScale)) *
				DirectX::XMMatrixRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&_objectAngle)) *
				DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&_intersectionWorldPoint)));

			_deformer->GetActor()->GetScene()->GetMeshRenderer().DrawTest(_selectingModel.get(), world);
		}
	}

	switch (_intersectState)
	{
	case IntersectState::Terrain:
		// 地形に接触していて左クリックしたら配置
		if (_selectingModel && _INPUT_RELEASED("LeftClick"))
		{
			// オブジェクトを配置
			_deformer->AddEnvironmentObject(
				_selectingModelPath,
				TerrainObjectLayout::CollisionType::None,
				_intersectionWorldPoint,
				_objectAngle,
				_objectScale);
		}
		break;
	}

}
void ObjectLayoutBrush::UpdateCreateObject(std::shared_ptr<Terrain> terrain, float elapsedTime)
{
}
void ObjectLayoutBrush::UpdateMoveObject(std::shared_ptr<Terrain> terrain, float elapsedTime)
{
}
void ObjectLayoutBrush::UpdateEditObject(std::shared_ptr<Terrain> terrain, float elapsedTime)
{
}
#pragma endregion
