#include "InGameCanvasActor.h"

#include "../../Library/Graphics/Graphics.h"

#include "UI/Timer/TimerUIController.h"
#include "UI/Chest/ChestUIController.h"
#include "UI/Item/ItemUIController.h"
#include "UI/Item/Slider/ItemSliderController.h"
#include "UI/Operate/OperateUIController.h"
#include "UI/Option/OptionUIActor.h"
#include "UI/Player/PlayerHealthUIController.h"
#include "UI/Player/PlayerStaminaUIController.h"
#include "UI/Player/PlayerBuffUIController.h"
#include "UI/Player/PlayerShapnessUIController.h"
#include "UI/Player/PlayerQuestOrderUIController.h"
#include "UI/Quest/QuestUIController.h"
#include "UI/QuestBoard/QuestBoardUIActor.h"

#include <Mygui.h>

// 生成時処理
void InGameCanvasActor::OnCreate()
{
	// 起動フラグの子への反映をオフ
	SetPropagateActiveChange(false);

    // タイマー生成
    auto timerUIActor = CreateUIActor<UIActor>("TimerUI");
    timerUIActor->AddComponent<TimerUIController>();

	// チェスト生成
	auto chestUIActor = CreateUIActor<UIActor>("ChestUI");
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
	auto itemUIActor = CreateUIActor<UIActor>("ItemUI");
	itemUIActor->GetRectTransform().SetLocalPosition(Vector2(1800.0f, 940.0f));
	itemUIActor->GetRectTransform().SetLocalScale(Vector2(0.8f, 0.8f));
	itemUIActor->AddComponent<ItemUIController>();
	{
		auto itemSliderActor = GetScene()->RegisterActor<UIActor>("ItemSliderUI", ActorTag::UI);
		itemSliderActor->SetParent(itemUIActor.get());
		itemSliderActor->AddComponent<ItemSliderController>();
	}

	// 操作UI生成
	auto operateUIActor = CreateUIActor<UIActor>("OperateUI");
	operateUIActor->AddComponent<OperateUIController>();

	// プレイヤーUI生成
	auto playerUIActor = CreateUIActor<UIActor>("PlayerUI");
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
	auto questBoardUIActor = CreateUIActor<QuestBoardUIActor>("QuestBoardUI");

	// クエストUI生成
	auto questUIActor = CreateUIActor<UIActor>("QuestUI");
	questUIActor->AddComponent<QuestUIController>();
	questUIActor->GetRectTransform().SetLocalPosition(Vector2(1950.0f, 340.0f));

	// オプションUI生成
	auto optionUIActor = CreateUIActor<OptionUIActor>("OptionUIActor");
}

// GUI描画
void InGameCanvasActor::OnDrawGui()
{
	static ImGuiTabBarFlags tab_bar_flags =
		ImGuiTabBarFlags_AutoSelectNewTabs |
		ImGuiTabBarFlags_Reorderable |
		ImGuiTabBarFlags_FittingPolicyResizeDown;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem(u8"キャンバス"))
		{
			for (auto& [name, flag] : _uiActiveMap)
			{
				bool isActive = flag;
				if (ImGui::Checkbox(name.c_str(), &isActive))
				{
					SetUIActive(name, isActive);
				}
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
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

// UI名前取得
std::vector<std::string> InGameCanvasActor::GetUIActorNames() const
{
	std::vector<std::string> names;
	for (const auto& [name, flag] : _uiActiveMap)
	{
		names.push_back(name);
	}
	return names;
}

// UI表示設定
void InGameCanvasActor::SetUIActive(const std::string& name, bool isActive)
{
	auto it = _uiActiveMap.find(name);
	if (it != _uiActiveMap.end())
	{
		it->second = isActive;
		for (auto& actor : this->GetChildren())
		{
			if (actor->GetName() == name)
			{
				actor->SetIsActive(isActive);
				break;
			}
		}
	}
}
