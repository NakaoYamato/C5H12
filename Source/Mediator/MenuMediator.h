#pragma once

#include "../../Library/Actor/Actor.h"
#include "../../Library/2D/Sprite.h"
#include "../../Library/Scene/Scene.h"

#include "../../Source/Menu/MenuInput.h"

#include <unordered_map>

class MenuObjectBase;
class MenuCategoryBase;
class MenuItemBase;

using MenuCategoryRef	= std::shared_ptr<MenuCategoryBase>;
using MenuItemRef		= std::shared_ptr<MenuItemBase>;

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
	void RegisterMenuCategory(MenuCategoryRef category);
	// メニューカテゴリー削除
	void UnregisterMenuCategory(MenuCategoryRef category);
	// メニューアイテム登録
	void RegisterMenuItemController(std::string categoryName, MenuItemRef controller);
	// メニューアイテム削除
	void UnregisterMenuItemController(std::string categoryName, MenuItemRef controller);

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
	std::shared_ptr<MenuInput> GetMenuInput() const
	{
		return _menuInput.lock();
	}
private:
	// MenuInputへの参照
	std::weak_ptr<MenuInput> _menuInput;
	// カテゴリーとアイテムの関係を記録したマップ
	std::vector<MenuPair> _menuMap;
	// メニューカテゴリー名とコントローラーのマップ
	std::unordered_map<std::string, MenuCategoryRef> _menuCategoryMap;
	// メニューアイテム名とコントローラーのマップ
	std::unordered_map<std::string, MenuItemRef> _menuItemMap;
	// 選択中のカテゴリー番号
	int _selectedCategoryIndex = 0;
	// 選択中のアイテム番号
	int _selectedItemIndex = 0;

	// カテゴリーのオフセット
	Vector2 _categoryOffset = Vector2(0.0f, 100.0f);
	// カテゴリーの間隔
	Vector2 _categoryInterval = Vector2(200.0f, 0.0f);
	// アイテムのオフセット
	Vector2 _itemOffset = Vector2(0.0f, 150.0f);
	// アイテムの間隔
	Vector2 _itemInterval = Vector2(0.0f, 50.0f);
};

class MenuObjectBase
{
public:
	MenuObjectBase() = delete;
	MenuObjectBase(MenuMediator* menuMediator, const std::string& menuName) :
		_menuMediator(menuMediator)
	{
		SetMenuName(menuName);
	}
	virtual ~MenuObjectBase() {}
	/// <summary>
	///	更新処理
	/// </summary>
	/// <param name="elapsedTime"></param>
	virtual void Update(float elapsedTime) {};
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="rc"></param>
	/// <param name="offset"></param>
	/// <param name="offsetScale"></param>
	virtual void Render(
		Scene* scene,
		const RenderContext& rc,
		const Vector2& offset,
		const Vector2& offsetScale) = 0;
	/// <summary>
	/// GUI描画
	/// </summary>
	virtual void DrawGui() {};

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
	// 選択状態を設定
	void SetSelected(bool isSelected)
	{
		_isSelected = isSelected;
	}
	// 選択状態を取得
	bool IsSelected() const
	{
		return _isSelected;
	}
protected:
	// MenuMediatorへの参照
	MenuMediator* _menuMediator;
	// メニュー名
	std::string _menuName;
	// スプライトのマップ
	std::unordered_map<std::string, Sprite> _sprites;

	// 選択されているか
	bool _isSelected = false;
};

class MenuCategoryBase : public MenuObjectBase
{
public:
	MenuCategoryBase(MenuMediator* menuMediator, const std::string& menuName) :
		MenuObjectBase(menuMediator, menuName) {
	}
	virtual ~MenuCategoryBase() {}

	// メニューアイテムのコマンドを実行
	virtual void ExecuteCommand(const std::string& command) = 0;
};

class MenuItemBase : public MenuObjectBase
{
public:
	MenuItemBase(MenuMediator* menuMediator, const std::string& menuName) :
		MenuObjectBase(menuMediator, menuName) {
	}
	virtual ~MenuItemBase() {}

	// メニューアイテムのコマンドを実行
	virtual void ExecuteCommand(const std::string& command) = 0;
};