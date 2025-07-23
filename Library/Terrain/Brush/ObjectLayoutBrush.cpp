#include "ObjectLayoutBrush.h"

#include "../../Library/Input/Input.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Scene/Scene.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"
#include "../../Library/Collision/CollisionMath.h"

#include "../../External/nameof/include/nameof.hpp"
#include "../../External/magic_enum/include/magic_enum/magic_enum.hpp"

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
	_deformer->GetActor()->SetIsUsingGuizmo(false);
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
	static std::vector<const char*> CollisionTypeNames;
	if (CollisionTypeNames.size() == 0)
	{
		for (auto type : magic_enum::enum_values<TerrainObjectLayout::CollisionType>())
		{
			CollisionTypeNames.push_back(magic_enum::enum_name(type).data());
		}
	}

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
	ImGui::Combo(u8"オブジェクトの当たり判定属性", reinterpret_cast<int*>(&_objectCollisionType), CollisionTypeNames.data(), static_cast<int>(CollisionTypeNames.size()));
	ImGui::DragFloat3(u8"オブジェクトのスケール", &_objectScale.x, 0.01f, 0.01f, 10.0f);
	Vector3 deg = _objectAngle.ToDegrees();
	if (ImGui::DragFloat3(u8"オブジェクトの角度", &deg.x, 0.1f))
		_objectAngle = deg.ToRadians();
	ImGui::DragFloat3(u8"オブジェクトの衝突オフセット", &_objectCollisionOffset.x, 0.01f);
	switch (_objectCollisionType)
	{
	case TerrainObjectLayout::CollisionType::None:
		break;
	case TerrainObjectLayout::CollisionType::Box:
	case TerrainObjectLayout::CollisionType::BoxTrigger:
		ImGui::DragFloat3(u8"オブジェクトの衝突半辺長", &_objectCollisionParameter.x, 0.01f);
		break;
	case TerrainObjectLayout::CollisionType::Sphere:
	case TerrainObjectLayout::CollisionType::SphereTrigger:
		ImGui::DragFloat(u8"オブジェクトの衝突半径", &_objectCollisionParameter.x, 0.01f);
		break;
	case TerrainObjectLayout::CollisionType::Capsule:
	case TerrainObjectLayout::CollisionType::CapsuleTrigger:
		ImGui::DragFloat3(u8"オブジェクトの衝突終点", &_objectCollisionParameter.x, 0.01f);
		ImGui::DragFloat(u8"オブジェクトの衝突半径", &_objectCollisionParameter.w, 0.01f);
		break;
	case TerrainObjectLayout::CollisionType::Mesh:
	case TerrainObjectLayout::CollisionType::MeshTrigger:
		break;
	default:
		break;
	}

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
		DirectX::XMFLOAT4X4 world{};
		DirectX::XMStoreFloat4x4(&world,
			DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&_objectScale)) *
			DirectX::XMMatrixRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&_objectAngle)) *
			DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&_intersectionWorldPoint)));

		if (_selectingModel)
		{
			_deformer->GetActor()->GetScene()->GetMeshRenderer().DrawTest(_selectingModel.get(), world);
		}

		// 当たり判定の描画
		switch (_objectCollisionType)
		{
		case TerrainObjectLayout::CollisionType::Box:
			Debug::Renderer::DrawBox(
				_objectCollisionOffset.TransformCoord(world),
				_objectAngle,
				_objectCollisionParameter.Vec3(),
				Vector4::Green);
			break;
		case TerrainObjectLayout::CollisionType::BoxTrigger:
			Debug::Renderer::DrawBox(
				_objectCollisionOffset.TransformCoord(world),
				_objectAngle,
				_objectCollisionParameter.Vec3(),
				Vector4::Blue);
			break;
		case TerrainObjectLayout::CollisionType::Sphere:
			Debug::Renderer::DrawSphere(
				_objectCollisionOffset.TransformCoord(world),
				_objectCollisionParameter.x,
				Vector4::Green);
			break;
		case TerrainObjectLayout::CollisionType::SphereTrigger:
			Debug::Renderer::DrawSphere(
				_objectCollisionOffset.TransformCoord(world),
				_objectCollisionParameter.x,
				Vector4::Blue);
			break;
		case TerrainObjectLayout::CollisionType::Capsule:
			Debug::Renderer::DrawCapsule(
				_objectCollisionOffset.TransformCoord(world),
				_objectCollisionParameter.Vec3().TransformCoord(world),
				_objectCollisionParameter.w,
				Vector4::Green);
			break;
		case TerrainObjectLayout::CollisionType::CapsuleTrigger:
			Debug::Renderer::DrawCapsule(
				_objectCollisionOffset.TransformCoord(world),
				_objectCollisionParameter.Vec3().TransformCoord(world),
				_objectCollisionParameter.w,
				Vector4::Blue);
			break;
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
				_objectCollisionType,
				_intersectionWorldPoint,
				_objectAngle,
				_objectScale,
				_objectCollisionOffset,
				_objectCollisionParameter);
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
