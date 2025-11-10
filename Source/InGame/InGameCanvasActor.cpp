#include "InGameCanvasActor.h"

#include "../../Library/Graphics/Graphics.h"

#include "../../Source/InGame/UI/TimerUIController.h"
#include "../../Source/InGame/UI/Chest/ChestUIController.h"
#include "../../Source/InGame/UI/Chest/Menu/ChestSelectMenuController.h"
#include "../../Source/InGame/UI/Chest/Menu/ChestArmorMenuController.h"

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
	chestUIActor->AddComponent<ChestUIController>();
	{
		auto SelectState = GetScene()->RegisterActor<UIActor>("ChestSelectStateUI", ActorTag::UI);
		SelectState->SetParent(chestUIActor.get());
        SelectState->AddComponent<ChestSelectMenuController>();

		auto ItemState = GetScene()->RegisterActor<UIActor>("ChestItemStateUI", ActorTag::UI);
		ItemState->SetParent(chestUIActor.get());

		auto ArmorState = GetScene()->RegisterActor<UIActor>("ChestArmorStateUI", ActorTag::UI);
		ArmorState->SetParent(chestUIActor.get());
		ArmorState->AddComponent<ChestArmorMenuController>();
	}
}
