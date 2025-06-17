#include "MenuMediator.h"

#include "../../Library/Scene/Scene.h"
#include <imgui.h>

// 生成時処理
void MenuMediator::OnCreate()
{


	SetActiveFlag(false);
}
// 遅延更新処理
void MenuMediator::OnLateUpdate(float elapsedTime)
{
}
// UI描画処理
void MenuMediator::OnDelayedRender(const RenderContext& rc)
{
	Vector2 offset = Vector2::Zero;
	Vector2 offsetScale = Vector2::One;

	// メニューカテゴリーの描画
	auto& [category, itemList] = _menuMap[_selectedCategoryIndex];
	_menuCategoryMap[category]->DrawUI(rc, offset, offsetScale);
	// メニューアイテムの描画
	int itemCount = 0;
	for (const auto& itemName : itemList)
	{
		auto it = _menuItemMap.find(itemName);
		if (it != _menuItemMap.end())
		{
			it->second->DrawUI(rc, offset, offsetScale);
			itemCount++;
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

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

// メニューカテゴリー登録
void MenuMediator::RegisterMenuCategory(MenuCategoryBase* category, std::string categoryName)
{
	_menuCategoryMap[categoryName] = category;
	category->SetMenuName(categoryName);
	_menuMap.push_back(MenuPair(categoryName, std::vector<std::string>()));
}
// メニューカテゴリー削除
void MenuMediator::UnregisterMenuCategory(MenuCategoryBase* category)
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
void MenuMediator::RegisterMenuItemController(std::string categoryName, MenuItemBase* controller, std::string itemName)
{
	auto it = _menuCategoryMap.find(categoryName);
	if (it != _menuCategoryMap.end())
	{
		_menuItemMap[itemName] = controller;
		controller->SetMenuName(itemName);
	}
	// カテゴリー名とアイテム名を追加
	auto menuIt = std::find_if(_menuMap.begin(), _menuMap.end(),
		[&categoryName](const MenuPair& pair) { return pair.first == categoryName; });
	if (menuIt != _menuMap.end())
	{
		menuIt->second.push_back(itemName);
	}
}
// メニューアイテム削除
void MenuMediator::UnregisterMenuItemController(std::string categoryName, MenuItemBase* controller)
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

// MenuMediatorを探す
std::weak_ptr<MenuMediator> MenuObjectBase::FindMenuMediator()
{
	auto scene = GetActor()->GetScene();
	if (scene == nullptr)
		return std::weak_ptr<MenuMediator>();
	return scene->GetActorManager().FindByClass<MenuMediator>(ActorTag::DrawContextParameter);
}
