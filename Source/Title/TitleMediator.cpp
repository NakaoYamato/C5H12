#include "TitleMediator.h"

#include "../../Source/Title/Item/TitleToHomeItem.h"
#include "../../Source/Title/Item/TitleToOnlineItem.h"
#include "../../Source/Title/Item/TitleToOfflineItem.h"
#include "../../Source/Title/Item/TitleToExitItem.h"
#include "../../Source/Title/Item/TitleToOnlineSettingItem.h"

#include "../../Library/Input/Input.h"
#include "../../Source/Scene/Game/SceneGame.h"

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
	static std::string NetworkAddress = "127.0.0.1";
	if (ImGui::Begin(u8"設定"))
	{
		ImGui::InputText(u8"ネットワークアドレス", &NetworkAddress);
		if (ImGui::Button(u8"決定"))
		{
			// ゲームシーンに移行
			SceneGame::NetworkEnabled = true;
			SceneGame::NetworkAddress = NetworkAddress;
			SceneManager::Instance().ChangeScene(SceneMenuLevel::Game, "Game");
		}
	}
	ImGui::End();

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
		if (item->IsActive())
			item->Update(elapsedTime);
	}
}
// UI描画処理
void TitleMediator::OnDelayedRender(const RenderContext& rc)
{
	// タイトルアイテムの描画
	for (const auto& item : _titleItems)
	{
		if (item->IsActive())
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
// コマンドを受信
void TitleMediator::ReceiveCommand(const std::string& sender, const std::string& target, const std::string& command, float delayTime)
{
	CommandData commandData;
	commandData.sender = sender;
	commandData.target = target;
	commandData.command = command;
	commandData.delayTime = delayTime;
	_commandList.push_back(commandData);
}
// タイトルアイテムの登録
void TitleMediator::RegisterTitleItem(TitleItemRef item)
{
	_titleItems.push_back(item);
}
// タイトルアイテムの削除
void TitleMediator::UnregisterTitleItem(TitleItemRef item)
{
	auto it = std::remove(_titleItems.begin(), _titleItems.end(), item);
	if (it != _titleItems.end())
	{
		_titleItems.erase(it, _titleItems.end());
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
			item->ExecuteCommand(command);
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
void TitleItemBase::ExecuteCommand(const TitleMediator::CommandData& commandData)
{
	if (commandData.command == TitleMediator::ActivateCommand)
	{
		_isActive = true;
	}
	else if (commandData.command == TitleMediator::DeactivateCommand)
	{
		_isActive = false;
	}
}
