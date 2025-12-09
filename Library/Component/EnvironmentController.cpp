#include "EnvironmentController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Graphics/Graphics.h"

#include <Mygui.h>

// 生成時処理
void EnvironmentController::OnCreate()
{
	_actorFactory = ResourceManager::Instance().GetResourceAs<ActorFactory>("ActorFactory");
	auto actorFactory = _actorFactory.lock();
	if (!actorFactory)
		return;

	// 編集で表示する用の環境アクター生成
	auto actorTypes = actorFactory->GetRegisteredActorTypes();
	for (size_t i = 0; i < actorTypes.size(); ++i)
	{
		std::string name = "EditingObj" + std::to_string(i);

		auto actor = actorFactory->CreateActor(
			GetActor()->GetScene(),
			actorTypes[i],
			name,
			ActorTag::Stage);
		actor->SetIsActive(false);
		actor->SetParent(GetActor().get());
		_editingEnvironmentActors.push_back(actor);
	}

	// データ読み込み
	LoadFromFile();
}

// 開始処理
void EnvironmentController::Start()
{
}

// 遅延更新処理
void EnvironmentController::LateUpdate(float elapsedTime)
{
	if (_isEditing)
	{
		// 編集用GUI描画
		DrawEditingGui();

		auto actorFactory = _actorFactory.lock();
		if (!actorFactory)
			return;

		int size = static_cast<int>(_editingEnvironmentActors.size());
		if (_selectedEnvironmentIndex < 0 || _selectedEnvironmentIndex >= size)
			return;


		// 入力更新
		_isDoubleClick = false;
		if (_INPUT_RELEASED("LeftClick"))
		{
			// ダブルクリック判定
			if (_leftClickTime < _doubleClickThreshold)
				_isDoubleClick = true;
			_leftClickTime = 0.0f;
		}
		_leftClickTime += elapsedTime;

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
		_intersectedActor = nullptr;
		auto actors = GetActor()->GetScene()->GetCollisionManager().RayCast(rayStart, rayDir, distance);
		if (actors.size() > 0)
		{
			// このオブジェクト以外と交差していたら交差点を保存
			for (auto& result : actors)
			{
				if (result.hitActor != this->GetActor().get())
				{
					_intersectionWorldPoint = result.hitPosition;
					_intersectedActor = result.hitActor;
					break;
				}
			}
		}

		// 編集で表示する用の環境アクターのみ表示
		for (int i = 0; i < size; ++i)
		{
			auto actor = _editingEnvironmentActors[i].lock();
			if (!actor)
				continue;
			if (_selectedEnvironmentIndex == i)
			{
				actor->SetIsActive(true);
				actor->GetTransform().SetPosition(_intersectionWorldPoint);
				actor->GetTransform().SetAngle(_creationAngle);
				actor->GetTransform().SetScale(_creationScale);
			}
			else
			{
				actor->SetIsActive(false);
			}
		}

		// ダブルクリックされたときにアクター生成
		if (_isDoubleClick)
		{
#ifdef USE_IMGUI
			//	ウィンドウにフォーカス中の場合は処理しない
			if (ImGui::IsAnyItemActive() || ImGui::IsAnyItemFocused() || ImGui::IsAnyItemHovered())
				return;
#endif

			auto actor = CreateEnvironmentActor(
				actorFactory->GetRegisteredActorTypes()[_selectedEnvironmentIndex],
				_intersectedActor ? _intersectedActor->GetName() : "",
				_intersectionWorldPoint,
				_creationAngle,
				_creationScale);
		}
	}
	else
	{
		// 編集で表示する用の環境アクター非表示
		int size = static_cast<int>(_editingEnvironmentActors.size());
		for (int i = 0; i < size; ++i)
		{
			auto actor = _editingEnvironmentActors[i].lock();
			if (!actor)
				continue;
			actor->SetIsActive(false);
		}
	}
}

// GUI描画
void EnvironmentController::DrawGui()
{
	ImGui::Checkbox(u8"編集中", &_isEditing);
	if (_isEditing)
	{
		ImGui::Separator();
		ImGui::DragFloat3("position", &_intersectionWorldPoint.x, 0.1f);
		ImGui::DragFloat3("scale", &_creationScale.x, 0.1f);
		Vector3 degree = Vector3::ToDegrees(_creationAngle);
		ImGui::DragFloat3("angle", &degree.x);
		_creationAngle = Vector3::ToRadians(degree);
	}
	ImGui::Separator();

	if (ImGui::TreeNode(u8"生成した環境アクターリスト"))
	{
		int index = 0;
		for (auto& layout : _createdEnvironmentLayouts)
		{
			int i = index++;

			auto actor = layout.actor.lock();
			if (!actor)
				continue;


			ImGui::PushID(i);

			ImGui::Text("%d: %s", i, actor->GetName());
			ImGui::DragFloat3("position", &layout.position.x, 0.1f);
			ImGui::DragFloat3("scale", &layout.scale.x, 0.1f);
			Vector3 degree = Vector3::ToDegrees(layout.angle);
			ImGui::DragFloat3("angle", &degree.x);
			layout.angle = Vector3::ToRadians(degree);
			if (ImGui::Button(u8"更新"))
			{
				actor->GetTransform().SetPosition(layout.position);
				actor->GetTransform().SetAngle(layout.angle);
				actor->GetTransform().SetScale(layout.scale);
			}
			ImGui::Separator();

			if (ImGui::Button(u8"削除"))
			{
				actor->Remove();
				// 配置情報リストから削除
				_createdEnvironmentLayouts.erase(
					_createdEnvironmentLayouts.begin() + i);
				ImGui::PopID();
				break;
			}

			ImGui::PopID();
		}
		ImGui::TreePop();
	}
}

#pragma region 入出力
// ファイル読み込み
bool EnvironmentController::LoadFromFile()
{
	// 現在登録している環境アクターを削除
	for (auto& layout : _createdEnvironmentLayouts)
	{
		auto actor = layout.actor.lock();
		if (actor)
		{
			actor->Remove();
		}
	}
	_createdEnvironmentLayouts.clear();

	// 現在のアクター名からファイルパスを生成
	std::string filePath = this->GetDirectory();
	filePath += this->GetName();
	filePath += ".json";

	nlohmann::json json;
	if (Exporter::LoadJsonFile(filePath.c_str(), &json))
	{
		// 環境アクター配置情報読み込み
		auto actorFactory = _actorFactory.lock();
		if (!actorFactory)
			return false;
		for (const auto& layoutJson : json["CreatedEnvironmentLayouts"])
		{
			std::string actorType = layoutJson.value("ActorType", "");
			std::string parent = layoutJson.value("Parent", "");
			Vector3 position = layoutJson.value("Position", Vector3());
			Vector3 angle = layoutJson.value("Angle", Vector3());
			Vector3 scale = layoutJson.value("Scale", Vector3(Vector3::One));
			auto actor = CreateEnvironmentActor(
				actorType,
				_intersectedActor ? _intersectedActor->GetName() : "",
				position,
				angle,
				scale);
			if (!actor)
				continue;
		}
		return true;
	}

	return false;
}

// ファイル保存
bool EnvironmentController::SaveToFile()
{
	// 現在のアクター名からファイルパスを生成
	std::string filePath = this->GetDirectory();
	filePath += this->GetName();
	filePath += ".json";

	nlohmann::json json;
	// 環境アクター配置情報保存
	json["CreatedEnvironmentLayouts"] = nlohmann::json::array();
	for (const auto& layout : _createdEnvironmentLayouts)
	{
		nlohmann::json layoutJson;
		layoutJson["ActorType"] = layout.actorType;
		layoutJson["Parent"] = layout.parent;
		layoutJson["Position"] = layout.position;
		layoutJson["Angle"] = layout.angle;
		layoutJson["Scale"] = layout.scale;
		json["CreatedEnvironmentLayouts"].push_back(layoutJson);
	}
	return Exporter::SaveJsonFile(filePath,	json);
}
#pragma endregion

// 環境アクター生成
std::shared_ptr<Actor> EnvironmentController::CreateEnvironmentActor(
	std::string actorType,
	std::string parent,
	const Vector3& position,
	const Vector3& angle,
	const Vector3& scale)
{
	auto actorFactory = _actorFactory.lock();
	if (!actorFactory)
		return nullptr;
	std::string name = GetActor()->GetName();
	name += std::to_string(_creationEnvironmentIndex++);

	auto actor = actorFactory->CreateActor(
		GetActor()->GetScene(),
		actorType,
		name,
		ActorTag::Stage);
	if (!actor)
		return nullptr;

	Matrix4X4 parentInverse = parentInverse.Identity;
	if (parent.empty())
		actor->SetParent(GetActor().get());
	else
	{
		auto parentActor = GetActor()->GetScene()->GetActorManager().FindByName(parent);
		if (parentActor)
		{
			actor->SetParent(parentActor.get());
			parentInverse = parentActor->GetTransform().GetMatrix().Inverse();
		}
		else 
			actor->SetParent(GetActor().get());
	}
	actor->GetTransform().UpdateTransform(&actor->GetParent()->GetTransform().GetMatrix());
	actor->GetTransform().SetPosition(position.TransformCoord(parentInverse));
	actor->GetTransform().SetAngle(angle);
	actor->GetTransform().SetScale(scale);

	EnvironmentLayout layout{};
	layout.actor = actor;
	layout.actorType = actorType;
	layout.parent = parent;
	layout.position = position;
	layout.angle = angle;
	layout.scale = scale;
	_createdEnvironmentLayouts.push_back(layout);

	return actor;
}

// 編集用GUI描画
void EnvironmentController::DrawEditingGui()
{
	auto actorFactory = _actorFactory.lock();
	if (!actorFactory)
		return;

	if (ImGui::Begin(u8"環境編集"))
	{
		// 環境アクター選択ラジオボタン
		auto actorTypes = actorFactory->GetRegisteredActorTypes();
		for (size_t i = 0; i < actorTypes.size(); ++i)
		{
			if (ImGui::RadioButton(actorTypes[i].c_str(), _selectedEnvironmentIndex == static_cast<int>(i)))
			{
				if (_selectedEnvironmentIndex != static_cast<int>(i))
					_selectedEnvironmentIndex = static_cast<int>(i);
				else
					_selectedEnvironmentIndex = -1;
			}
		}
	}
	ImGui::End();
}
