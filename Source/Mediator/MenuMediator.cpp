#include "MenuMediator.h"

#include "../../Library/Scene/Scene.h"
#include <imgui.h>

#include "../../Source/Menu/Category/MenuCategory.h"
#include "../../Source/Menu/Item/MenuItem.h"
#include "../../Source/Menu/Item/MenuExitItem.h"

#pragma region コマンド名
const char* MenuMediator::ActivateCommand = "Activate";
const char* MenuMediator::DeactivateCommand = "Deactivate";
#pragma endregion
#pragma region 対象
const char* MenuMediator::AllCategory = "AllCategory";
const char* MenuMediator::AllItem = "AllItem";
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
		RegisterMenuItemController(category0Category->GetMenuName(), item0Item);
		auto item1Item = std::make_shared<MenuItem>(this, u8"Item1");
		RegisterMenuItemController(category0Category->GetMenuName(), item1Item);
	}
	auto systemCategory = std::make_shared<MenuCategory>(this, u8"System");
	RegisterMenuCategory(systemCategory);
	{
		auto item0Item = std::make_shared<MenuItem>(this, u8"Item0");
		RegisterMenuItemController(systemCategory->GetMenuName(), item0Item);
		auto exitItem = std::make_shared<MenuExitItem>(this, u8"Exit");
		RegisterMenuItemController(systemCategory->GetMenuName(), exitItem);
	}
}
// 遅延更新処理
void MenuMediator::OnLateUpdate(float elapsedTime)
{
	// メニュー入力がアクティブでない場合は何もしない
	if (_menuInput.lock() == nullptr || !_menuInput.lock()->IsActive())
		return;

	// 各カテゴリーの更新
	for (auto& category : _categoryMap)
	{
		if (category->IsActive())
			category->Update(elapsedTime);
	}
	// 各アイテムの更新
	for (auto& [categoryName, itemList] : _categoryItemsMap)
	{
		for (auto& item : itemList)
		{
			if (item->IsActive())
				item->Update(elapsedTime);
		}
	}

	//if (_menuMap.size() <= _selectedCategoryIndex)
	//	return;

	//// アイテムを開いていない場合、入力処理を行う
	//if (!_isItemOpen)
	//{
	//	if (_menuInput.lock()->IsInput(MenuInput::InputType::Left))
	//		_selectedCategoryIndex--;
	//	if (_menuInput.lock()->IsInput(MenuInput::InputType::Right))
	//		_selectedCategoryIndex++;
	//	if (_menuInput.lock()->IsInput(MenuInput::InputType::Up))
	//		_selectedItemIndex--;
	//	if (_menuInput.lock()->IsInput(MenuInput::InputType::Down))
	//		_selectedItemIndex++;

	//	if (_menuInput.lock()->IsInput(MenuInput::InputType::Back))
	//		_menuInput.lock()->CloseMenu();
	//}

	//// 選択中のカテゴリーインデックスを範囲内に調整
	//if (_selectedCategoryIndex < 0)
	//	_selectedCategoryIndex = static_cast<int>(_menuMap.size()) - 1;
	//if (_selectedCategoryIndex >= static_cast<int>(_menuMap.size()))
	//	_selectedCategoryIndex = 0;
	//// 選択中のアイテムインデックスを範囲内に調整
	//if (_menuMap.at(_selectedCategoryIndex).second.size() > 0)
	//{
	//	if (_selectedItemIndex < 0)
	//		_selectedItemIndex = static_cast<int>(_menuMap.at(_selectedCategoryIndex).second.size()) - 1;
	//	if (_selectedItemIndex >= static_cast<int>(_menuMap.at(_selectedCategoryIndex).second.size()))
	//		_selectedItemIndex = 0;
	//}

	//// 選択中のカテゴリーのアイテムを強調表示
	//_isItemOpen = false;
	//for (auto& [categoryName, itemNameList] : _menuMap)
	//{
	//	auto category = _menuCategoryMap.find(categoryName);
	//	if (category == _menuCategoryMap.end())
	//		continue;

	//	// 選択中のカテゴリーを選択状態にし、選択外のカテゴリーを非選択状態にする
	//	if (categoryName == _menuMap[_selectedCategoryIndex].first)
	//	{
	//		category->second->SetSelected(true);
	//		category->second->Update(elapsedTime);

	//		// 選択中のアイテムを選択状態にし、選択外のアイテムを非選択状態にする
	//		for (size_t i = 0; i < itemNameList.size(); ++i)
	//		{
	//			auto it = _menuItemMap.find(itemNameList[i]);
	//			if (it != _menuItemMap.end())
	//			{
	//				if (i == _selectedItemIndex)
	//				{
	//					it->second->SetSelected(true);

	//					// 開いていない場合入力によって開く
	//					if (!it->second->IsOpen())
	//					{
	//						if (_menuInput.lock()->IsInput(MenuInput::InputType::Select))
	//						{
	//							it->second->SetOpen(true);
	//						}
	//					}
	//					else
	//					{
	//						_isItemOpen = true;
	//						it->second->Update(elapsedTime);
	//						if (_menuInput.lock()->IsInput(MenuInput::InputType::Back))
	//						{
	//							it->second->SetOpen(false);
	//						}
	//					}
	//				}
	//				else
	//				{
	//					it->second->SetSelected(false);
	//				}
	//			}
	//		}
	//	}
	//	else
	//	{
	//		category->second->SetSelected(false);
	//	}
	//}
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
		if (category->IsActive())
		{
			category->Render(GetScene(), rc, categoryOffset, Vector2::One);
			categoryOffset += _categoryInterval;
		}
	}
	// 各アイテムの更新
	for (auto& [categoryName, itemList] : _categoryItemsMap)
	{
		Vector2 itemOffset = _itemOffset;
		for (auto& item : itemList)
		{
			if (item->IsActive())
			{
				item->Render(GetScene(), rc, categoryOffset, Vector2::One);
				itemOffset += _itemInterval;
			}
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
					if (ImGui::TreeNode(category->GetMenuName().c_str()))
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
							if (ImGui::TreeNode(item->GetMenuName().c_str()))
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
	_categoryItemsMap[category->GetMenuName()].clear();
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
// コマンドを実行
void MenuMediator::ExecuteCommand(const CommandData& command)
{
}
