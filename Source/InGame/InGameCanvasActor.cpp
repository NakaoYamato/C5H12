#include "InGameCanvasActor.h"

#include "../../Library/Graphics/Graphics.h"

#include "UI/TimerUIController.h"
#include "UI/Chest/ChestUIController.h"
#include "UI/Item/ItemUIController.h"
#include "UI/Item/Slider/ItemSliderController.h"
#include "UI/Operate/OperateUIController.h"
#include "UI/Menu/MenuUIController.h"
#include "UI/Player/PlayerHealthUIController.h"
#include "UI/Player/PlayerStaminaUIController.h"
#include "UI/Player/PlayerBuffUIController.h"
#include "UI/Player/PlayerShapnessUIController.h"
#include "UI/Player/PlayerQuestOrderUIController.h"
#include "UI/Quest/QuestUIController.h"
#include "UI/QuestBoard/QuestBoardUIActor.h"

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

	// プレイヤーUI生成
	auto playerUIActor = GetScene()->RegisterActor<UIActor>("PlayerUI", ActorTag::UI);
	playerUIActor->SetParent(this);
	{
		auto playerHPUIActor = GetScene()->RegisterActor<UIActor>("PlayerHPUI", ActorTag::UI);
		playerHPUIActor->SetParent(playerUIActor.get());
		playerHPUIActor->GetRectTransform().SetLocalPosition(Vector2(50.0f, 50.0f));
		playerHPUIActor->GetRectTransform().SetLocalScale(Vector2(1.0f, 0.5f));
		playerHPUIActor->AddComponent<PlayerHealthUIController>();

		auto playerStaminaUIActor = GetScene()->RegisterActor<UIActor>("PlayerStaminaUI", ActorTag::UI);
		playerStaminaUIActor->SetParent(playerUIActor.get());
		playerStaminaUIActor->GetRectTransform().SetLocalPosition(Vector2(50.0f, 74.0f));
		playerStaminaUIActor->GetRectTransform().SetLocalScale(Vector2(1.0f, 0.5f));
		playerStaminaUIActor->AddComponent<PlayerStaminaUIController>();

		auto playerBuffUIActor = GetScene()->RegisterActor<UIActor>("PlayerBuffUI", ActorTag::UI);
		playerBuffUIActor->SetParent(playerUIActor.get());
		playerBuffUIActor->AddComponent<PlayerBuffUIController>();

		auto playerShapnessUIActor = GetScene()->RegisterActor<UIActor>("PlayerShapnessUI", ActorTag::UI);
		playerShapnessUIActor->SetParent(playerUIActor.get());
		playerShapnessUIActor->AddComponent<PlayerShapnessUIController>();

		auto playerQuestOrderUIActor = GetScene()->RegisterActor<UIActor>("PlayerQuestOrderUI", ActorTag::UI);
		playerQuestOrderUIActor->SetParent(playerUIActor.get());
		playerQuestOrderUIActor->AddComponent<PlayerQuestOrderUIController>();
	}

	// クエストボードUI生成
	auto questBoardUIActor = GetScene()->RegisterActor<QuestBoardUIActor>("QuestBoardUI", ActorTag::UI);
	questBoardUIActor->SetParent(this);

	// クエストUI生成
	auto questUIActor = GetScene()->RegisterActor<UIActor>("QuestUI", ActorTag::UI);
	questUIActor->SetParent(this);
	questUIActor->AddComponent<QuestUIController>();
	questUIActor->GetRectTransform().SetLocalPosition(Vector2(1950.0f, 340.0f));
}

// プレイヤー設定
void InGameCanvasActor::RegisterPlayerActor(Actor* playerActor)
{
	Actor* playerUIActor = nullptr;
	for (auto& actor : this->GetChildren())
	{
		std::string name = actor->GetName();
		if (name == "PlayerUI")
		{
			playerUIActor = actor.get();
			break;
		}
	}
	if (!playerUIActor)
		return;

	// 各UIコントローラーにプレイヤーのコンポーネントを設定
	for (auto& child : playerUIActor->GetChildren())
	{
		if (auto healthUIController = child->GetComponent<PlayerHealthUIController>())
		{
			auto damageable = playerActor->GetComponent<Damageable>();
			healthUIController->SetDamageable(damageable);
		}
		else if (auto staminaUIController = child->GetComponent<PlayerStaminaUIController>())
		{
			auto staminaController = playerActor->GetComponent<StaminaController>();
			staminaUIController->SetStaminaController(staminaController);
		}
		else if (auto buffUIController = child->GetComponent<PlayerBuffUIController>())
		{
			auto buffController = playerActor->GetComponent<BuffController>();
			buffUIController->SetBuffController(buffController);
		}
		else if (auto shapnessUIController = child->GetComponent<PlayerShapnessUIController>())
		{
			for (auto& weaponActor : playerActor->GetChildren())
			{
				if (auto sharpnessController = weaponActor->GetComponent<SharpnessController>())
				{
					shapnessUIController->SetSharpnessController(sharpnessController);
					break;
				}
			}
		}
		else if (auto questOrderUIController = child->GetComponent<PlayerQuestOrderUIController>())
		{
			questOrderUIController->SetPlayerActor(playerActor->shared_from_this());
		}
	}
}
