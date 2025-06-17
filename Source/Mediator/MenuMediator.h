#pragma once

#include "../../Library/Actor/Actor.h"
#include "../../Library/Component/Component.h"

#include "../../Library/Component/SpriteRenderer.h"

#include <unordered_map>

class MenuObjectBase;
class MenuCategoryBase;
class MenuItemBase;

using MenuCategoryRef = MenuCategoryBase*;
using MenuItemRef = MenuItemBase*;

class MenuMediator : public Actor
{
public:
	// Key: カテゴリー名, Value: アイテム名のリスト
	using MenuPair = std::pair<std::string, std::vector<std::string>>;

public:
	~MenuMediator() override {}
	// 生成時処理
	void OnCreate() override;
	// 遅延更新処理
	void OnLateUpdate(float elapsedTime) override;
	// UI描画処理
	void OnDelayedRender(const RenderContext& rc) override;
	// GUI描画
	void OnDrawGui() override;

	// MenuCategoryControllerBaseからの命令を受信
	//void ReceiveCommand(const MenuCategoryBase* sender, const std::string& command);

	// メニューカテゴリー登録
	void RegisterMenuCategory(MenuCategoryBase* category, std::string categoryName);
	// メニューカテゴリー削除
	void UnregisterMenuCategory(MenuCategoryBase* category);
	// メニューアイテム登録
	void RegisterMenuItemController(std::string categoryName, MenuItemBase* controller, std::string itemName);
	// メニューアイテム削除
	void UnregisterMenuItemController(std::string categoryName, MenuItemBase* controller);

	const MenuPair* GetMenuPair(const std::string& categoryName) const
	{
		for (const auto& pair : _menuMap)
		{
			if (pair.first == categoryName)
			{
				return &pair;
			}
		}
		return nullptr; // 失敗
	}
private:
	// カテゴリーとアイテムの関係を記録したマップ
	std::vector<MenuPair> _menuMap;
	// メニューカテゴリー名とコントローラーのマップ
	std::unordered_map<std::string, MenuCategoryRef> _menuCategoryMap;
	// メニューアイテム名とコントローラーのマップ
	std::unordered_map<std::string, MenuItemRef> _menuItemMap;
	// 選択中のカテゴリー番号
	int _selectedCategoryIndex = 0;
};

class MenuObjectBase : public SpriteRenderer
{
public:
	// 3D描画後の描画処理
	// ※メディエーターで制御するため処理しない
	void DelayedRender(const RenderContext& rc) override {}
	/// <summary>
	/// UI描画
	/// </summary>
	/// <param name="rc"></param>
	/// <param name="offset"></param>
	/// <param name="offsetScale"></param>
	virtual void DrawUI(
		const RenderContext& rc,
		const Vector2& offset,
		const Vector2& offsetScale) = 0;

	// メニュー名を設定
	void SetMenuName(const std::string& menuName)
	{
		_menuName = menuName;
	}
	// メニュー名を取得
	const std::string& GetMenuName() const
	{
		return _menuName;
	}
protected:
	// MenuMediatorを探す
	std::weak_ptr<MenuMediator> FindMenuMediator();
protected:
	// MenuMediatorへの参照
	std::weak_ptr<MenuMediator> _menuMediator;
	// メニュー名
	std::string _menuName;
};

class MenuCategoryBase : public MenuObjectBase
{
public:
	MenuCategoryBase() {}
	virtual ~MenuCategoryBase() {}

	// メニューアイテムのコマンドを実行
	virtual void ExecuteCommand(const std::string& command) = 0;
};

class MenuItemBase : public MenuObjectBase
{
public:
	MenuItemBase() {}
	virtual ~MenuItemBase() {}

	// メニューアイテムのコマンドを実行
	virtual void ExecuteCommand(const std::string& command) = 0;
};