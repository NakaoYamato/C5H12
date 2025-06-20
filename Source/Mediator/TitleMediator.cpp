#include "TitleMediator.h"

#include "../../Source/Title/Item/TitleToHomeItem.h"
#include "../../Source/Title/Item/TitleToOnlineItem.h"
#include "../../Source/Title/Item/TitleToOfflineItem.h"
#include "../../Source/Title/Item/TitleToExitItem.h"
#include "../../Source/Title/Item/TitleToOnlineSettingItem.h"

#include <imgui.h>

#pragma region コマンド名
const char* TitleMediator::ActivateCommand		= "Activate";
const char* TitleMediator::DeactivateCommand	= "Deactivate";
#pragma endregion

#pragma region アイテム名
const char* TitleMediator::AllItem				= "AllItem";
const char* TitleMediator::ToHomeItem			= "ToHomeItem";
const char* TitleMediator::ToOnlineItem			= "ToOnlineItem";
const char* TitleMediator::ToOfflineItem		= "ToOfflineItem";
const char* TitleMediator::ToSettingItem		= "ToSettingItem";
const char* TitleMediator::ToExitItem			= "ToExitItem";
const char* TitleMediator::ToOnlineSettingItem	= "ToOnlineSettingItem";
#pragma endregion

// 生成時処理
void TitleMediator::OnCreate()
{

	// タイトルアイテムの登録
	RegisterTitleItem(std::make_shared<TitleToHomeItem>(this, ToHomeItem));
	RegisterTitleItem(std::make_shared<TitleToOnlineItem>(this, ToOnlineItem));
	RegisterTitleItem(std::make_shared<TitleToOfflineItem>(this, ToOfflineItem));
	RegisterTitleItem(std::make_shared<TitleToExitItem>(this, ToExitItem));
	RegisterTitleItem(std::make_shared<TitleToOnlineSettingItem>(this, ToOnlineSettingItem));

	ReceiveCommand(this->GetName(), TitleMediator::AllItem, TitleMediator::DeactivateCommand, 0.0f);
	ReceiveCommand(this->GetName(), TitleMediator::ToHomeItem, TitleMediator::ActivateCommand, 0.0f);
}
// 遅延更新処理
void TitleMediator::OnLateUpdate(float elapsedTime)
{
	// コマンドの処理
	for (auto& command : _commandList)
	{
		// 経過時間処理
		command.delayTime -= elapsedTime;

		// コマンドの実行
		if (command.delayTime <= 0.0f)
		{
			ExecuteCommand(command);
			// コマンドを削除
			auto it = std::find_if(_commandList.begin(), _commandList.end(),
				[&command](const CommandData& cmd) { return &cmd == &command; });
			if (it != _commandList.end())
			{
				_commandList.erase(it);
			}
		}
	}

	// タイトルアイテムの更新
	for (const auto& item : _titleItems)
	{
		item->Update(elapsedTime);
	}
}
// UI描画処理
void TitleMediator::OnDelayedRender(const RenderContext& rc)
{
	// タイトルアイテムの描画
	for (const auto& item : _titleItems)
	{
		item->Render(GetScene(), rc);
	}
}
// GUI描画
void TitleMediator::OnDrawGui()
{
	static ImGuiTabBarFlags tab_bar_flags =
		ImGuiTabBarFlags_AutoSelectNewTabs |
		ImGuiTabBarFlags_Reorderable |
		ImGuiTabBarFlags_FittingPolicyResizeDown;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem(u8"アイテム"))
		{
			// タイトルアイテムのGUI描画
			for (const auto& item : _titleItems)
			{
				if (ImGui::TreeNode(item->GetName().c_str()))
				{
					item->DrawGui();
					ImGui::TreePop();
				}
			}

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(u8"コマンドリスト"))
		{
			// コマンドリストのGUI描画
			for (const auto& command : _commandList)
			{
				ImGui::Text("Sender: %s", command.sender.c_str());
				ImGui::Text("Target: %s", command.target.c_str());
				ImGui::Text("Delay: %.2f", command.delayTime);
				ImGui::Separator();
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}
// コマンドを実行
void TitleMediator::ExecuteCommand(const CommandData& command)
{
	// タイトルアイテムの検索
	for (const auto& item : _titleItems)
	{
		if (command.target == item->GetName() || 
			command.target == TitleMediator::AllItem)
		{
			// コマンドを実行
			item->ExecuteCommand(command.command);
		}
	}
}
// GUI描画
void TitleItemBase::DrawGui()
{
	ImGui::Checkbox("Active", &_isActive);
	ImGui::Separator();
	// スプライトのGUI描画
	for (auto& [name, spriteData] : _sprites)
	{
		if (ImGui::TreeNode(name.c_str()))
		{
			spriteData.DrawGui();
			ImGui::TreePop();
		}
	}
}
// コマンドを実行
void TitleItemBase::ExecuteCommand(const std::string& command)
{
	if (command == TitleMediator::ActivateCommand)
	{
		_isActive = true;
	}
	else if (command == TitleMediator::DeactivateCommand)
	{
		_isActive = false;
	}
}
