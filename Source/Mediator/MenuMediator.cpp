#include "MenuMediator.h"

#include "../../Library/Scene/Scene.h"
#include <imgui.h>

#include "../../Source/Menu/Category/MenuCategory.h"
#include "../../Source/Menu/Item/MenuItem.h"
#include "../../Source/Menu/Item/ExitMenuItem.h"

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
	auto category1Category = std::make_shared<MenuCategory>(this, u8"Category1");
	RegisterMenuCategory(category1Category);
	{
		auto item0Item = std::make_shared<MenuItem>(this, u8"Item0");
		RegisterMenuItemController(category1Category->GetMenuName(), item0Item);
		auto exitItem = std::make_shared<ExitMenuItem>(this, u8"Exit");
		RegisterMenuItemController(category1Category->GetMenuName(), exitItem);
	}
}
// 遅延更新処理
void MenuMediator::OnLateUpdate(float elapsedTime)
{
	if (_menuInput.lock() == nullptr || !_menuInput.lock()->IsActive())
		return;
	if (_menuMap.size() <= _selectedCategoryIndex)
		return;

	if (_menuInput.lock()->IsInput(MenuInput::InputType::Left))
		_selectedCategoryIndex--;
	if (_menuInput.lock()->IsInput(MenuInput::InputType::Right))
		_selectedCategoryIndex++;
	if (_menuInput.lock()->IsInput(MenuInput::InputType::Up))
		_selectedItemIndex--;
	if (_menuInput.lock()->IsInput(MenuInput::InputType::Down))
		_selectedItemIndex++;


	// 選択中のカテゴリーインデックスを範囲内に調整
	if (_selectedCategoryIndex < 0)
		_selectedCategoryIndex = static_cast<int>(_menuMap.size()) - 1;
	if (_selectedCategoryIndex >= static_cast<int>(_menuMap.size()))
		_selectedCategoryIndex = 0;
	// 選択中のアイテムインデックスを範囲内に調整
	if (_menuMap.at(_selectedCategoryIndex).second.size() > 0)
	{
		if (_selectedItemIndex < 0)
			_selectedItemIndex = static_cast<int>(_menuMap.at(_selectedCategoryIndex).second.size()) - 1;
		if (_selectedItemIndex >= static_cast<int>(_menuMap.at(_selectedCategoryIndex).second.size()))
			_selectedItemIndex = 0;
	}

	// 選択中のカテゴリーのアイテムを強調表示
	for (auto& [categoryName, itemNameList] : _menuMap)
	{
		auto category = _menuCategoryMap.find(categoryName);
		if (category == _menuCategoryMap.end())
			continue;

		// 選択中のカテゴリーを選択状態にし、選択外のカテゴリーを非選択状態にする
		if (categoryName == _menuMap[_selectedCategoryIndex].first)
		{
			category->second->SetSelected(true);
			category->second->Update(elapsedTime);

			// 選択中のアイテムを選択状態にし、選択外のアイテムを非選択状態にする
			for (size_t i = 0; i < itemNameList.size(); ++i)
			{
				auto it = _menuItemMap.find(itemNameList[i]);
				if (it != _menuItemMap.end())
				{
					if (i == _selectedItemIndex)
					{
						it->second->SetSelected(true);
						it->second->Update(elapsedTime);
					}
					else
					{
						it->second->SetSelected(false);
					}
				}
			}
		}
		else
		{
			category->second->SetSelected(false);
		}
	}
}
// UI描画処理
void MenuMediator::OnDelayedRender(const RenderContext& rc)
{
	if (_menuInput.lock() == nullptr || !_menuInput.lock()->IsActive())
		return;
	if (_menuMap.size() <= _selectedCategoryIndex)
		return;

	size_t categoryCount = _menuMap.size();
	for (size_t i = 0; i < categoryCount; ++i)
	{
		Vector2 categoryOffset = _categoryOffset + _categoryInterval * static_cast<float>(i);
		Vector2 categoryOffsetScale = Vector2::One;
		auto& [category, itemList] = _menuMap[i];

		if (!category.empty())
		{
			// カテゴリーの描画
			_menuCategoryMap[category]->Render(GetScene(), rc, categoryOffset, categoryOffsetScale);

			// カテゴリーのアイテムを描画
			if (i == _selectedCategoryIndex)
			{
				Vector2 itemOffset = _itemOffset;
				Vector2 itemOffsetScale = Vector2::One;
				for (const auto& itemName : itemList)
				{
					auto it = _menuItemMap.find(itemName);
					if (it != _menuItemMap.end())
					{
						it->second->Render(GetScene(), rc, itemOffset, itemOffsetScale);
						itemOffset += _itemInterval; // アイテム間隔を適用
					}
				}
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
			ImGui::DragInt(u8"選択中カテゴリー", &_selectedCategoryIndex, 1.0f, 0, static_cast<int>(_menuMap.size() - 1));
			ImGui::DragFloat2(u8"カテゴリーオフセット", &_categoryOffset.x, 1.0f, -1000.0f, 1000.0f);
			ImGui::DragFloat2(u8"カテゴリー間隔", &_categoryInterval.x, 1.0f, 0.0f, 1000.0f);
			ImGui::DragFloat2(u8"アイテムオフセット", &_itemOffset.x, 1.0f, -1000.0f, 1000.0f);
			ImGui::DragFloat2(u8"アイテム間隔", &_itemInterval.x, 1.0f, -1000.0f, 1000.0f);

			ImGui::Separator();
			if (ImGui::TreeNode(u8"メニューカテゴリー"))
			{
				for (const auto& [categoryName, category] : _menuCategoryMap)
				{
					if (ImGui::TreeNode(categoryName.c_str()))
					{
						// カテゴリーのGUI描画
						category->DrawGui();
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode(u8"メニューアイテム"))
			{
				for (const auto& [itemName, item] : _menuItemMap)
				{
					if (ImGui::TreeNode(itemName.c_str()))
					{
						// アイテムのGUI描画
						item->DrawGui();
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

// メニューカテゴリー登録
void MenuMediator::RegisterMenuCategory(MenuCategoryRef category)
{
	_menuCategoryMap[category->GetMenuName()] = category;
	_menuMap.push_back(MenuPair(category->GetMenuName(), std::vector<std::string>()));
}
// メニューカテゴリー削除
void MenuMediator::UnregisterMenuCategory(MenuCategoryRef category)
{
	auto it = _menuCategoryMap.find(category->GetMenuName());
	if (it != _menuCategoryMap.end())
	{
		_menuCategoryMap.erase(it);
	}
	// カテゴリー名を削除
	auto menuIt = std::remove_if(_menuMap.begin(), _menuMap.end(),
		[&category](const MenuPair& pair) { return pair.first == category->GetMenuName(); });
}
// メニューアイテム登録
void MenuMediator::RegisterMenuItemController(std::string categoryName, MenuItemRef controller)
{
	auto it = _menuCategoryMap.find(categoryName);
	if (it != _menuCategoryMap.end())
	{
		_menuItemMap[controller->GetMenuName()] = controller;
	}
	// カテゴリー名とアイテム名を追加
	auto menuIt = std::find_if(_menuMap.begin(), _menuMap.end(),
		[&categoryName](const MenuPair& pair) { return pair.first == categoryName; });
	if (menuIt != _menuMap.end())
	{
		menuIt->second.push_back(controller->GetMenuName());
	}
}
// メニューアイテム削除
void MenuMediator::UnregisterMenuItemController(std::string categoryName, MenuItemRef controller)
{
	auto it = _menuItemMap.find(controller->GetMenuName());
	if (it != _menuItemMap.end())
	{
		_menuItemMap.erase(it);
	}
	// アイテム名を削除
	auto itemIt = std::find_if(_menuMap.begin(), _menuMap.end(),
		[&categoryName](const MenuPair& pair) { return pair.first == categoryName; });
	if (itemIt != _menuMap.end())
	{
		itemIt->second.erase(std::remove(itemIt->second.begin(), itemIt->second.end(), controller->GetMenuName()), itemIt->second.end());
	}
}
