#include "ChestController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Graphics/Graphics.h"

#include <imgui.h>

// 初期化処理
void ChestController::Start()
{
	_interactionController = GetActor()->GetComponent<InteractionController>();
	// コールバック設定
	if (auto interactionController = _interactionController.lock())
	{
		// 選択時
		interactionController->RegisterOnSelectCallback(
			"ChestController",
			[&](Actor* target)
			{
				// すでに開いている場合は処理しない
				auto chestInput = _chestInput.lock();
				if (!chestInput || chestInput->IsActive())
					return;

				// 頭上にアイコン表示
				float screenWidth = Graphics::Instance().GetScreenWidth();
				float screenHeight = Graphics::Instance().GetScreenHeight();
				Vector3 worldPosition = this->GetActor()->GetTransform().GetWorldPosition() + _selectUIOffset;
				auto& rc = this->GetActor()->GetScene()->GetRenderContext();
				Vector3 project = worldPosition.Project(screenWidth, screenHeight,
					rc.camera->GetView(), rc.camera->GetProjection());
				if (project.z > 0.0f)
				{
					InputUI::DrawInfo info;
					info.position = { project.x, project.y };
					info.keyboardKey = 'F';
					info.gamePadKey = XINPUT_GAMEPAD_A;
					info.scale = Vector2::One;
					info.isActive = false;
					info.color = Vector4::Blue;
					GetActor()->GetScene()->GetInputUI()->Draw(info);
				}
			}
		);
		// 使用可能か
		interactionController->RegisterIsUsableCallback(
			"ChestController",
			[&](Actor* target) -> bool
			{
				// すでに開いている場合は使用不可
				auto chestInput = _chestInput.lock();
				if (!chestInput || chestInput->IsActive())
					return false;
				return true;
			}
		);
		// 使用時
		interactionController->RegisterOnUseCallback(
			"ChestController",
			[&](Actor* target)
			{
				auto chestInput = _chestInput.lock();
				if (!chestInput)
					return;

				// すでに開いている場合は何もしない
				if (chestInput->IsActive())
					return;

				chestInput->Swich();
				if (auto chestUI = chestInput->GetChestUIController().lock())
				{
					// 使用中のチェストアクター設定
					chestUI->SetChestActor(GetActor());
				}

				return;
			}
		);
	}

	// 入力マネージャーの子供からチェスト入力コンポーネント取得
	if (auto inputManager = GetActor()->GetScene()->GetActorManager().FindByClass<InputManager>(ActorTag::System))
	{
		for (auto& child : inputManager->GetChildren())
		{
			if (auto chestInput = child->GetComponent<ChestInput>())
			{
				_chestInput = chestInput;
				break;
			}
		}
	}

	// 子供アクター取得
	for (auto& child : GetActor()->GetChildren())
	{
		if (std::string(child->GetName()).find("Top") != std::string::npos)
		{
			_topActor = child;
			break;
		}
	}
}

// 更新処理
void ChestController::Update(float elapsedTime)
{
	auto chestInput = _chestInput.lock();
	if (!chestInput)
		return;

	auto topActor = _topActor.lock();
	if (!topActor)
		return;

	float topActorAngle = topActor->GetTransform().GetAngle().x;
	if (chestInput->IsActive())
		topActorAngle = MathF::Lerp(topActorAngle, _openAngleX, elapsedTime * _angleSpeed);
	else
		topActorAngle = MathF::Lerp(topActorAngle, 0.0f, elapsedTime * _angleSpeed);

	topActor->GetTransform().SetAngleX(topActorAngle);
}

// GUI描画処理
void ChestController::DrawGui()
{
	if (auto chestInput = _chestInput.lock())
	{
		bool f = chestInput->IsActive();
		ImGui::Checkbox(u8"開いているか", &f);
	}
	ImGui::DragFloat(u8"開く角度(度)", &_openAngleX, 1.0f, 0.0f, 90.0f);
	ImGui::DragFloat(u8"開く速度(度/s)", &_angleSpeed, 1.0f, 1.0f, 360.0f);
	ImGui::DragFloat3(u8"選択UIオフセット", &_selectUIOffset.x);
}
