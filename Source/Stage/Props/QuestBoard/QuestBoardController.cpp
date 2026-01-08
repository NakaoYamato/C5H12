#include "QuestBoardController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Graphics/Graphics.h"

#include <imgui.h>

// 初期化処理
void QuestBoardController::Start()
{
	_interactionController = GetActor()->GetComponent<InteractionController>();
	// コールバック設定
	if (auto interactionController = _interactionController.lock())
	{
		// 選択時
		interactionController->RegisterOnSelectCallback(
			"QuestBoardController",
			[&](Actor* target)
			{
				// すでに開いている場合は処理しない
				auto questBoardInput = _questBoardInput.lock();
				if (!questBoardInput || questBoardInput->IsActive())
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
			"QuestBoardController",
			[&](Actor* target) -> bool
			{
				auto questBoardInput = _questBoardInput.lock();
				if (!questBoardInput)
					return false;
				// すでに開いている場合は使用不可
				if (questBoardInput->IsActive())
					return false;
				return true;
			}
		);
		// 使用時
		interactionController->RegisterOnUseCallback(
			"QuestBoardController",
			[&](Actor* target)
			{
				auto questBoardInput = _questBoardInput.lock();
				if (!questBoardInput)
					return;
				// すでに開いている場合は何もしない
				if (questBoardInput->IsActive())
					return;
				questBoardInput->Swich();
				return;
			}
		);
	}

	// 入力マネージャーの子供からチェスト入力コンポーネント取得
	if (auto inputManager = GetActor()->GetScene()->GetActorManager().FindByClass<InputManager>(ActorTag::System))
	{
		for (auto& child : inputManager->GetChildren())
		{
			if (auto questBoardInput = child->GetComponent<QuestBoardInput>())
			{
				_questBoardInput = questBoardInput;
				break;
			}
		}
	}
}

// 更新処理
void QuestBoardController::Update(float elapsedTime)
{
}

// GUI描画処理
void QuestBoardController::DrawGui()
{
	ImGui::DragFloat3(u8"選択UIオフセット", &_selectUIOffset.x);
}
