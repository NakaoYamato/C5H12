#include "MenuMediator.h"

#include "../../Library/Scene/Scene.h"
#include <imgui.h>

#include "../../Source/Menu/Category/MenuCategory.h"
#include "../../Source/Menu/Item/MenuItem.h"
#include "../../Source/Menu/Item/MenuExitItem.h"

#pragma region コマンド名
const char* MenuMediator::ActivateCommand		= "Activate";
const char* MenuMediator::DeactivateCommand		= "Deactivate";

const char* MenuMediator::ResetCommand			= "Reset";
const char* MenuMediator::SelectionCommand		= "Selection";
const char* MenuMediator::OpenCommand			= "Open";
const char* MenuMediator::CloseCommand			= "Close";
const char* MenuMediator::EscapeCommand			= "Escape";
#pragma endregion
#pragma region 対象
const char* MenuMediator::ToAllObject		= "ToAllObject";

const char* MenuMediator::ToNextCategory	= "ToNextCategory";
const char* MenuMediator::ToBackCategory	= "ToBackCategory";
const char* MenuMediator::ToItem			= "ToItem";
const char* MenuMediator::ToNextItem		= "ToNextItem";
const char* MenuMediator::ToBackItem		= "ToBackItem";
#pragma endregion

// 生成時処理
void MenuMediator::OnCreate()
{
	_menuInput = this->AddComponent<MenuInput>();

	// メニュー構築
	auto category0Category = std::make_shared<MenuCategory>(this, u8"Category0");
	RegisterMenuCategory(category0Category);
	{
		auto item0Item = std::make_shared<MenuItem>(this, u8"Item0");
		RegisterMenuItemController(category0Category->GetName(), item0Item);
		auto item1Item = std::make_shared<MenuItem>(this, u8"Item1");
		RegisterMenuItemController(category0Category->GetName(), item1Item);
	}
	auto systemCategory = std::make_shared<MenuCategory>(this, u8"System");
	RegisterMenuCategory(systemCategory);
	{
		auto item0Item = std::make_shared<MenuItem>(this, u8"Item0");
		RegisterMenuItemController(systemCategory->GetName(), item0Item);
		auto exitItem = std::make_shared<MenuExitItem>(this, u8"Exit");
		RegisterMenuItemController(systemCategory->GetName(), exitItem);
	}

	ReceiveCommand(
		"MenuMediator",
		category0Category->GetName(),
		MenuMediator::ResetCommand,
		0.0f);
}
// 遅延更新処理
void MenuMediator::OnLateUpdate(float elapsedTime)
{
	// メニュー入力がアクティブでない場合は何もしない
	if (_menuInput.lock() == nullptr || !_menuInput.lock()->IsActive())
		return;

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

	// 選択中のカテゴリーの更新
	if (_currentCategoryIndex != -1)
	{
		auto& category = _categoryMap.at(_currentCategoryIndex);

		bool isItemOpen = false;
		// 選択中のカテゴリーのアイテムを更新
		auto& itemMap = _categoryItemsMap[category->GetName()];
		if (!itemMap.empty() || _currentItemIndex != -1)
		{
			if (itemMap[_currentItemIndex]->IsOpen())
			{
				itemMap[_currentItemIndex]->Update(elapsedTime);
				isItemOpen = true;
			}
		}
		// アイテムが開かれていない場合はカテゴリーを更新
		if (!isItemOpen)
		{
			category->Update(elapsedTime);
		}
	}
}
// UI描画処理
void MenuMediator::OnDelayedRender(const RenderContext& rc)
{
	// メニュー入力がアクティブでない場合は何もしない
	if (_menuInput.lock() == nullptr || !_menuInput.lock()->IsActive())
		return;

	// 各カテゴリーの描画
	Vector2 categoryOffset = _categoryOffset;
	for (auto& category : _categoryMap)
	{
		category->Render(GetScene(), rc, categoryOffset, Vector2::One);
		categoryOffset += _categoryInterval;
	}
	// 各アイテムの描画
	if (_currentCategoryIndex != -1)
	{
		auto& category = _categoryMap.at(_currentCategoryIndex);
		Vector2 itemOffset = _itemOffset;
		for (auto& item : _categoryItemsMap[category->GetName()])
		{
			item->Render(GetScene(), rc, itemOffset, Vector2::One);
			itemOffset += _itemInterval;
		}
	}
}
// GUI描画
void MenuMediator::OnDrawGui()
{
	static ImGuiTabBarFlags tab_bar_flags =
		ImGuiTabBarFlags_AutoSelectNewTabs |
		ImGuiTabBarFlags_Reorderable |
		ImGuiTabBarFlags_FittingPolicyResizeDown;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem(u8"メニュー"))
		{
			ImGui::DragFloat2(u8"カテゴリーオフセット", &_categoryOffset.x, 1.0f, -1000.0f, 1000.0f);
			ImGui::DragFloat2(u8"カテゴリー間隔", &_categoryInterval.x, 1.0f, 0.0f, 1000.0f);
			ImGui::DragFloat2(u8"アイテムオフセット", &_itemOffset.x, 1.0f, -1000.0f, 1000.0f);
			ImGui::DragFloat2(u8"アイテム間隔", &_itemInterval.x, 1.0f, -1000.0f, 1000.0f);

			ImGui::Separator();
			if (ImGui::TreeNode(u8"カテゴリー"))
			{
				for (auto& category : _categoryMap)
				{
					if (ImGui::TreeNode(category->GetName().c_str()))
					{
						// カテゴリーのGUI描画
						category->DrawGui();
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode(u8"アイテム"))
			{
				for (const auto& [categoryName, items] : _categoryItemsMap)
				{
					if (ImGui::TreeNode(categoryName.c_str()))
					{
						for (auto& item : items)
						{
							if (ImGui::TreeNode(item->GetName().c_str()))
							{
								// アイテムのGUI描画
								item->DrawGui();
								ImGui::TreePop();
							}
						}
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}
// コマンドを受信
void MenuMediator::ReceiveCommand(const std::string& sender, const std::string& target, const std::string& command, float delayTime)
{
	// コマンドデータを作成
	CommandData commandData;
	commandData.sender = sender;
	commandData.target = target;
	commandData.command = command;
	commandData.delayTime = delayTime;
	// コマンドリストに追加
	_commandList.push_back(commandData);
}
// メニューカテゴリー登録
void MenuMediator::RegisterMenuCategory(MenuCategoryRef category)
{
	_categoryMap.push_back(category);
}
// メニューカテゴリー削除
void MenuMediator::UnregisterMenuCategory(MenuCategoryRef category)
{
	// カテゴリーに含まれるアイテムを削除
	_categoryItemsMap[category->GetName()].clear();
	// カテゴリー自体を削除
	auto it = std::find(_categoryMap.begin(), _categoryMap.end(), category);
	if (it != _categoryMap.end())
	{
		_categoryMap.erase(it);
	}
}
// メニューアイテム登録
void MenuMediator::RegisterMenuItemController(std::string categoryName, MenuItemRef controller)
{
	_categoryItemsMap[categoryName].push_back(controller);
}
// メニューアイテム削除
void MenuMediator::UnregisterMenuItemController(std::string categoryName, MenuItemRef controller)
{
	auto itemMap = _categoryItemsMap.find(categoryName);
	if (itemMap != _categoryItemsMap.end())
	{
		auto& itemList = itemMap->second;
		auto it = std::remove(itemList.begin(), itemList.end(), controller);
		if (it != itemList.end())
		{
			itemList.erase(it, itemList.end());
		}
	}
}
// 名前から番号取得
int MenuMediator::FindCategoryIndex(const std::string& target)
{
	int index = 0;
	for (const auto& category : _categoryMap)
	{
		if (category->GetName() == target)
		{
			return index;
		}
		index++;
	}
	return -1;
}
// 名前から番号取得
int MenuMediator::FindItemIndex(const std::string& category, const std::string& target)
{
	int index = 0;
	for (const auto& item : _categoryItemsMap[category])
	{
		if (item->GetName() == target)
		{
			return index;
		}
		index++;
	}
	return -1;
}
// コマンドを実行
void MenuMediator::ExecuteCommand(const CommandData& command)
{
#pragma region 起動コマンドor停止コマンド
	if (command.command == ActivateCommand || command.command == DeactivateCommand)
	{
		// メディエーター側からは行わない
		assert(!"メディエーター側からは起動コマンドor停止コマンドを行わない");
	}
#pragma endregion

#pragma region リセットコマンド
	if (command.command == ResetCommand)
	{
		// すべてのカテゴリーを非アクティブ化
		for (auto& category : _categoryMap)
		{
			category->ExecuteCommand({
				command.sender,
				category->GetName(),
				DeactivateCommand,
				0.0f
				});
		}
		// すべてのアイテムを非アクティブ化
		for (auto& [categoryName, itemList] : _categoryItemsMap)
		{
			for (auto& item : itemList)
			{
				item->ExecuteCommand({
					command.sender,
					item->GetName(),
					DeactivateCommand,
					0.0f
					});
			}
		}

		// 先頭のカテゴリーをアクティブ化
		_categoryMap[0]->ExecuteCommand({
			command.sender,
			_categoryMap[0]->GetName(),
			ActivateCommand,
			0.0f
			});
		// 起動しているカテゴリーを更新
		_currentCategoryIndex = 0;

		// 先頭のアイテムを選択
		auto& itemMap = _categoryItemsMap[_categoryMap[0]->GetName()];
		if (!itemMap.empty())
		{
			itemMap[0]->ExecuteCommand({
				command.sender,
				itemMap[0]->GetName(),
				ActivateCommand,
				0.0f
				});
			// 起動しているアイテムを更新
			_currentItemIndex = 0;
		}
		else
		{
			_currentItemIndex = -1; // アイテムがない場合は-1に設定
		}
	}
#pragma endregion


#pragma region 選択コマンド
	if (command.command == SelectionCommand)
	{
		// 次のカテゴリーを選択
		if (command.target == ToNextCategory)
		{
			// 次のカテゴリーのインデックスを計算
			int mapSize = static_cast<int>(_categoryMap.size());
			int nextIndex = (_currentCategoryIndex + 1) % mapSize;
			// 選択中のカテゴリーを非選択状態にする
			_categoryMap[_currentCategoryIndex]->ExecuteCommand({
				command.sender,
				command.sender,
				DeactivateCommand,
				0.0f
				});
			// 次のカテゴリーを選択状態にする
			_categoryMap[nextIndex]->ExecuteCommand({
				command.sender,
				_categoryMap[nextIndex]->GetName(),
				ActivateCommand,
				0.0f
				});
			// 起動しているカテゴリーを更新
			_currentCategoryIndex = nextIndex;
		}
		// 前のカテゴリーを選択
		else if (command.target == ToBackCategory)
		{
			// 前のカテゴリーのインデックスを計算
			int mapSize = static_cast<int>(_categoryMap.size());
			int nextIndex = (_currentCategoryIndex + mapSize - 1) % mapSize;
			// 選択中のカテゴリーを非選択状態にする
			_categoryMap[_currentCategoryIndex]->ExecuteCommand({
				command.sender,
				command.sender,
				DeactivateCommand,
				0.0f
				});
			// 前のカテゴリーを選択状態にする
			_categoryMap[nextIndex]->ExecuteCommand({
				command.sender,
				_categoryMap[nextIndex]->GetName(),
				ActivateCommand,
				0.0f
				});
			// 起動しているカテゴリーを更新
			_currentCategoryIndex = nextIndex;
		}
		// 次のアイテムを対象
		else if (command.target == ToNextItem)
		{
			// 起動しているカテゴリーが空の場合は何もしない
			if (_currentCategoryIndex == -1)
				return;
			// アイテムマップ取得
			auto& category = _categoryMap.at(_currentCategoryIndex);
			auto& itemMap = _categoryItemsMap[category->GetName()];

			// 次のアイテムのインデックスを計算
			int mapSize = static_cast<int>(itemMap.size());
			int nextIndex = (_currentItemIndex + 1) % mapSize;
			// 選択中のアイテムを非選択状態にする
			itemMap.at(_currentItemIndex)->ExecuteCommand({
				command.sender,
				itemMap.at(_currentItemIndex)->GetName(),
				DeactivateCommand,
				0.0f
				});
			// 次のアイテムを選択状態にする
			itemMap.at(nextIndex)->ExecuteCommand({
				command.sender,
				itemMap.at(nextIndex)->GetName(),
				ActivateCommand,
				0.0f
				});
			// 起動しているアイテムを更新
			_currentItemIndex = nextIndex;
		}
		// 前のアイテムを対象
		else if (command.target == ToBackItem)
		{
			// 起動しているカテゴリーが空の場合は何もしない
			if (_currentCategoryIndex == -1)
				return;
			// アイテムマップ取得
			auto& category = _categoryMap.at(_currentCategoryIndex);
			auto& itemMap = _categoryItemsMap[category->GetName()];

			// 前のアイテムのインデックスを計算
			int mapSize = static_cast<int>(itemMap.size());
			int nextIndex = (_currentItemIndex + mapSize - 1) % mapSize;
			// 選択中のアイテムを非選択状態にする
			itemMap.at(_currentItemIndex)->ExecuteCommand({
				command.sender,
				itemMap.at(_currentItemIndex)->GetName(),
				DeactivateCommand,
				0.0f
				});
			// 前のアイテムを選択状態にする
			itemMap.at(nextIndex)->ExecuteCommand({
				command.sender,
				itemMap.at(nextIndex)->GetName(),
				ActivateCommand,
				0.0f
				});
			// 起動しているアイテムを更新
			_currentItemIndex = nextIndex;
		}
	}
#pragma endregion

#pragma region 開閉コマンド
	if (command.command == OpenCommand || command.command == CloseCommand)
	{
		// アイテムの処理
		if (command.target == ToItem)
		{
			// 起選択しているカテゴリーが空の場合は何もしない
			if (_currentCategoryIndex == -1)
				return;
			// 選択しているアイテムがなければ何もしない
			if (_currentItemIndex == -1)
				return;

			// 現在選択しているアイテムを開閉
			auto& category = _categoryMap.at(_currentCategoryIndex);
			auto& itemMap = _categoryItemsMap[category->GetName()];
			auto& item = itemMap[_currentItemIndex];
			if (item == nullptr)
				return;
			item->ExecuteCommand({
				command.sender,
				item->GetName(),
				command.command,
				0.0f
				});
			return;
		}
	}
#pragma endregion

#pragma region 終了コマンド
	if (command.command == EscapeCommand)
	{
		// 入力コントローラーに終了処理を送る
		_menuInput.lock()->CloseMenu();
	}
#pragma endregion

}
/// メニューアイテムのコマンドを実行
void MenuObjectBase::ExecuteCommand(const MenuMediator::CommandData& command)
{
	if (command.target == GetName())
	{
		if (command.command == MenuMediator::ActivateCommand)
		{
			SetActive(true);
		}
		else if (command.command == MenuMediator::DeactivateCommand)
		{
			SetActive(false);
		}
	}
}
/// メニューアイテムのコマンドを実行
void MenuItemBase::ExecuteCommand(const MenuMediator::CommandData& command)
{
	MenuObjectBase::ExecuteCommand(command);
	if (command.target == GetName())
	{
		if (command.command == MenuMediator::OpenCommand)
		{
			SetOpen(true);
		}
		else if (command.command == MenuMediator::CloseCommand)
		{
			SetOpen(false);
		}
	}
}
