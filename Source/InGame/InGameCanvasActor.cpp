#include "InGameCanvasActor.h"

#include "../../Library/Graphics/Graphics.h"

#include "UI/TimerUIController.h"

#include "UI/Chest/ChestUIController.h"

#include "UI/Item/ItemUIController.h"
#include "UI/Item/Slider/ItemSliderController.h"

#include "UI/Operate/OperateUIController.h"

#include "UI/Menu/MenuUIController.h"

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
		ItemState->AddComponent<ChestItemMenuController>();

		auto ArmorState = GetScene()->RegisterActor<UIActor>("ChestArmorStateUI", ActorTag::UI);
		ArmorState->SetParent(chestUIActor.get());
		ArmorState->AddComponent<ChestArmorMenuController>();
	}

	// アイテムUI生成
	auto itemUIActor = GetScene()->RegisterActor<UIActor>("ItemUI", ActorTag::UI);
	itemUIActor->GetRectTransform().SetLocalPosition(Vector2(1800.0f, 940.0f));
	itemUIActor->GetRectTransform().SetLocalScale(Vector2(0.8f, 0.8f));
	itemUIActor->SetParent(this);
	itemUIActor->AddComponent<ItemUIController>();
	{
		auto itemSliderActor = GetScene()->RegisterActor<UIActor>("ItemSliderUI", ActorTag::UI);
		itemSliderActor->SetParent(itemUIActor.get());
		itemSliderActor->AddComponent<ItemSliderController>();
	}

	// 操作UI生成
	auto operateUIActor = GetScene()->RegisterActor<UIActor>("OperateUI", ActorTag::UI);
	operateUIActor->SetParent(this);
	operateUIActor->AddComponent<OperateUIController>();

	// メニューUI生成
    auto menuUIActor = GetScene()->RegisterActor<UIActor>("MenuUI", ActorTag::UI);
    menuUIActor->SetParent(this);
    menuUIActor->AddComponent<MenuUIController>();
}
