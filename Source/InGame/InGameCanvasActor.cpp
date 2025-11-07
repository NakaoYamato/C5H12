#include "InGameCanvasActor.h"

#include "../../Library/Graphics/Graphics.h"

#include "../../Source/InGame/UI/TimerUIController.h"
#include "../../Source/InGame/UI/Chest/ChestUIController.h"

// 生成時処理
void InGameCanvasActor::OnCreate()
{
    // タイマー生成
    auto timerUIActor = GetScene()->RegisterActor<UIActor>("TimerUI", ActorTag::UI);
    timerUIActor->SetParent(this);
    auto timerUIController = timerUIActor->AddComponent<TimerUIController>();

	// チェスト生成
	auto chestUIActor = GetScene()->RegisterActor<UIActor>("ChestUI", ActorTag::UI);
	chestUIActor->SetParent(this);
	chestUIActor->GetRectTransform().SetLocalPosition(Vector2(1500.0f, 170.0f));
	chestUIActor->AddComponent<ChestUIController>();
	{
		auto SelectState = GetScene()->RegisterActor<UIActor>("ChestSelectStateUI", ActorTag::UI);
		SelectState->SetParent(chestUIActor.get());

		auto ItemState = GetScene()->RegisterActor<UIActor>("ChestItemStateUI", ActorTag::UI);
		ItemState->SetParent(chestUIActor.get());

		auto ArmorState = GetScene()->RegisterActor<UIActor>("ChestArmorStateUI", ActorTag::UI);
		ArmorState->SetParent(chestUIActor.get());
	}
}
