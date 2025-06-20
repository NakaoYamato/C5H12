#pragma once

#include "../../Library/Actor/Actor.h"
#include "../../Library/2D/Sprite.h"
#include "../../Library/Scene/Scene.h"

#include "../../Source/Menu/MenuInput.h"

#include <unordered_map>

#pragma region 前方宣言
class MenuObjectBase;
class MenuCategoryBase;
class MenuItemBase;

using MenuCategoryRef = std::shared_ptr<MenuCategoryBase>;
using MenuItemRef = std::shared_ptr<MenuItemBase>;
#pragma endregion

class MenuMediator : public Actor
{
public:
	// Key: カテゴリー名, Value: アイテム名のリスト
	using MenuPair = std::pair<std::string, std::vector<std::string>>;

	struct CommandData
	{
		// 送信者
		std::string sender;
		// 対象
		std::string target;
		// コマンドの種類
		std::string command;
		// 遅延時間（秒）
		float delayTime = 0.0f;
	};

#pragma region コマンド名
	static const char* ActivateCommand;
	static const char* DeactivateCommand;
#pragma endregion

#pragma region 対象
	static const char* AllCategory;
	static const char* AllItem;

#pragma endregion

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

	// コマンドを受信
	void ReceiveCommand(const std::string& sender, const std::string& target, const std::string& command, float delayTime = 0.0f);
	// メニューカテゴリー登録
	void RegisterMenuCategory(MenuCategoryRef category);
	// メニューカテゴリー削除
	void UnregisterMenuCategory(MenuCategoryRef category);
	// メニューアイテム登録
	void RegisterMenuItemController(std::string categoryName, MenuItemRef controller);
	// メニューアイテム削除
	void UnregisterMenuItemController(std::string categoryName, MenuItemRef controller);
	// 入力コントローラー取得
	std::shared_ptr<MenuInput> GetMenuInput() const { return _menuInput.lock(); }
private:
	// コマンドを実行
	void ExecuteCommand(const CommandData& command);

private:
	// MenuInputへの参照
	std::weak_ptr<MenuInput> _menuInput;
	// カテゴリーマップ
	std::vector<MenuCategoryRef> _categoryMap;
	// カテゴリーごとのアイテムマップ
	std::unordered_map<std::string, std::vector<MenuItemRef>> _categoryItemsMap;
	// コマンドリスト
	std::vector<CommandData> _commandList;

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
	// アクティブ状態を取得
	bool IsActive() const
	{
		return _isActive;
	}
	// アクティブ状態を設定
	void SetActive(bool isActive)
	{
		_isActive = isActive;
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
	// アクティブかどうか
	bool _isActive = true;
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

	// アイテムが開いているかどうかを取得
	bool IsOpen() const
	{
		return _isOpen;
	}
	// アイテムが開いているかどうかを設定
	void SetOpen(bool isOpen)
	{
		_isOpen = isOpen;
	}
private:
	// アイテムを開いているかどうか
	bool _isOpen = false;
};