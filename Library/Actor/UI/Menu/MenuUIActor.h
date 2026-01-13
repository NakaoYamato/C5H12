#pragma once

#include <stack>
#include <functional>

#include "../UIActor.h"
#include "../../Library/2D/Sprite.h"

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
	// 3D描画後の描画時処理
	void OnDelayedRender(const RenderContext& rc) override;
	// トランスフォーム更新
	void UpdateTransform() override;

	// GUI描画時処理
	void OnDrawGui() override;

	// 新しいページをスタックの一番上に積む
	void PushPage(std::unique_ptr<MenuWidget> page);
	// 一番上のページを破棄して戻る
	void PopPage();
private:
	std::stack<std::unique_ptr<MenuWidget>> _widgetStack;
};

// メニューUIのウィジェット基底クラス
class MenuWidget 
{
public:
	// オプションが選択された時のコールバック関数
	using OnSelectCallback = std::function<void(MenuUIActor*)>;

	struct Option
	{
		std::string label{};
		OnSelectCallback onSelect{};
	};

public:
    MenuWidget(std::string name) : _name(name) {}
    virtual ~MenuWidget() = default;

#pragma region 共通インターフェース
	// 開始処理
	virtual void OnEnter() {}
	// 終了処理
	virtual void OnExit() {}
	// 更新処理
	virtual void Update(float elapsedTime, MenuUIActor* owner);
	// 描画処理
	virtual void Render(const RenderContext& rc, MenuUIActor* owner);
	// GUI描画処理
	virtual void DrawGui(MenuUIActor* owner);
#pragma endregion

#pragma region オプション
	void AddOption(const std::string& label, const OnSelectCallback& onSelect)
	{
		_options.push_back({ label, onSelect });
	}
#pragma endregion

#pragma region アクセサ
	std::string GetName() const { return _name; }

	RectTransform& GetRectTransform() { return _rectTransform; }
#pragma endregion

protected:
    std::string _name;
	RectTransform _rectTransform;

	// オプションリスト
	std::vector<Option> _options;
	// オプションの背景画像
	Sprite _optionSprite;
	// 選択中のオプション画像
	Sprite _selectedOptionSprite;
	// オプション間の垂直間隔
	float _optionVerticalSpacing = 40.0f;
	// ラベルのフォントサイズ
	Vector2 _optionFontSize = Vector2::One;
	// ラベルのオフセット
	Vector2 _optionLabelOffset = Vector2(20.0f, 10.0f);
	// ラベルの色
	Vector4 _optionLabelColor = Vector4::White;
	// ラベルの選択中の色
	Vector4 _optionLabelSelectedColor = Vector4::Red;

	// 選択中のオプションインデックス
	size_t _selectedOptionIndex = 0;
};