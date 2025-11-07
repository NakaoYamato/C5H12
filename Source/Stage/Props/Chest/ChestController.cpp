#include "ChestController.h"

#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// 初期化処理
void ChestController::Start()
{
	// 入力マネージャーの子供からチェスト入力コンポーネント取得
	if (auto inputManager = GetActor()->GetScene()->GetActorManager().FindByClass<InputManager>(ActorTag::System))
	{
		for (auto& child : inputManager->GetChildren())
		{
			auto chestInput = child->GetComponent<ChestInput>();
			if (chestInput)
			{
				_chestInput = chestInput;
				break;
			}
		}
	}

	// 子供のアクター取得
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
}

bool ChestController::Open()
{
	auto chestInput = _chestInput.lock();
	if (!chestInput)
		return false;

	if (chestInput->IsActive())
		return false;

	chestInput->Swich();
	return true;
}

