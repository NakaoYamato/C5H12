#pragma once

#include "../UIActor.h"
#include "../../Library/Component/SpriteRenderer.h"
#include "../../Library/Algorithm/CallBack/CallBack.h"

class MenuWidget;

class MenuUIActor : public UIActor
{
public:
	MenuUIActor() = default;
	~MenuUIActor() override {}
	// 生成時処理
	void OnCreate() override;
	// 更新時処理
	void OnUpdate(float elapsedTime) override;
	// GUI描画時処理
	void OnDrawGui() override;

	// 起動フラグが変化したときの処理
	void OnChangedActive(bool isActive) override;

	// ルートウィジェット取得
	std::shared_ptr<MenuWidget> GetRootWidget() { return _rootWidget; }
	// スプライトレンダラー取得
	std::shared_ptr<SpriteRenderer> GetSpriteRenderer() { return _spriteRenderer.lock(); }
private:
	std::weak_ptr<SpriteRenderer> _spriteRenderer;

	std::shared_ptr<MenuWidget> _rootWidget;
	std::vector<MenuWidget*> _selectableWidgets;
	MenuWidget* _focusedWidget = nullptr;
};

class MenuWidget : public std::enable_shared_from_this<MenuWidget>
{
public:
	// 子供の選択方法
	enum class ChildSelectType
	{
		// 水平
		Horizontal,
		// 垂直
		Vertical,
		// なし
		None,
	};

public:
	MenuWidget() = default;
	virtual ~MenuWidget() = default;

	virtual void Setup();
	virtual void Update(float elapsedTime);
	virtual void Render(const RenderContext& rc);
	virtual void DrawGui();

	virtual bool CanActive();
	virtual bool CanSelect();

	virtual void OnActive();
	virtual void OnSelect();
	virtual void OnDeactive();

	// ファイル読み込み
	virtual bool LoadFromFile(nlohmann::json* json) const;
	// ファイル保存
	virtual bool SaveToFile(nlohmann::json* json);

	/// <summary>
	/// オーナー設定
	/// </summary>
	/// <param name="owner"></param>
	virtual void SetOwner(MenuUIActor* owner) { _owner = owner; }
	/// <summary>
	/// 子供追加
	/// </summary>
	/// <param name="name"></param>
	/// <param name="spriteNames"></param>
	/// <returns></returns>
	virtual std::shared_ptr<MenuWidget> AddChild(
		const std::string& name,
		const std::vector<std::string>& spriteNames);

#pragma region アクセサ
	MenuUIActor* GetOwner() const { return _owner; }
	std::shared_ptr<MenuWidget> GetParent() const { return _parent.lock(); }
	const std::vector<std::weak_ptr<MenuWidget>>& GetChildren() const { return _children; }
	const std::string& GetName() const { return _name; }
	const std::vector<std::string>& GetSpriteNames() const { return _spriteNames; }
	bool IsActive() const { return _isActive; }
	ChildSelectType GetChildSelectType() const { return _childSelectType; }

	void SetSpriteNames(const std::vector<std::string>& spriteNames) { _spriteNames = spriteNames; }
	void SetChildSelectType(ChildSelectType type) { _childSelectType = type; }
#pragma endregion

private:
	MenuUIActor* _owner = nullptr;

	std::weak_ptr<MenuWidget> _parent;
	std::vector<std::weak_ptr<MenuWidget>> _children;

	std::string _name{};
	std::vector<std::string> _spriteNames;

	// アクティブ状態かどうか
	bool _isActive = true;
	// 子供の選択方法
	ChildSelectType _childSelectType = ChildSelectType::Vertical;

#pragma region コールバック
	CallBackHandler<bool, MenuWidget*> _canActive;
	CallBackHandler<bool, MenuWidget*> _canSelect;

	CallBackHandler<void, MenuWidget*> _onSetup;
	CallBackHandler<void, MenuWidget*, float> _onUpdate;
	CallBackHandler<void, MenuWidget*, const RenderContext&> _onRender;
	CallBackHandler<void, MenuWidget*> _onActive;
	CallBackHandler<void, MenuWidget*> _onSelect;
	CallBackHandler<void, MenuWidget*> _onDeactive;
#pragma endregion
};
