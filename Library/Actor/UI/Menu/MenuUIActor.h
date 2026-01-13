#pragma once

#include <stack>

#include "../UIActor.h"
#include "../../Library/2D/Sprite.h"
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
	// 3D描画後の描画時処理
	void OnDelayedRender(const RenderContext& rc) override;
	// GUI描画時処理
	void OnDrawGui() override;

	// ウィジェット登録
	void RegisterWidget(std::shared_ptr<MenuWidget> widget);
	// コールバック登録
	void RegisterOptionSelectedCallback(const std::string& name, CallBack<void, MenuUIActor*> callback);
	// 新しいページをスタックの一番上に積む
	void PushPage(std::string name);
	// 一番上のページを破棄して戻る
	void PopPage();
	// オプションが選択された時のコールバックを呼び出す
	void CallOptionSelected(const std::string& callbackName);

#pragma region アクセサ
	// 現在のウィジェットを取得
	MenuWidget* GetCurrentWidget();
	// スタック取得
	std::stack<std::string> GetWidgetStack() const { return _widgetStackNames; }
	// 登録ウィジェット取得
	const std::unordered_map<std::string, std::shared_ptr<MenuWidget>>& GetRegisteredWidgets() const { return _registeredWidgets; }
	// コールバックハンドラ取得
	CallBackHandler<void, MenuUIActor*>& GetOptionSelectedCallbackHandler() { return onOptionSelected; }
#pragma endregion

#pragma region ファイル
	// ファイル読み込み
	virtual void LoadFromFile(const char* filepath);
	// ファイル保存
	virtual void SaveToFile(const char* filepath);
#pragma endregion

private:
	// GUI描画処理
	void DrawWidgetGui();

private:
	// ウィジェットスタック
	std::stack<std::string> _widgetStackNames;

	// 登録しているウィジェット
	std::unordered_map<std::string, std::shared_ptr<MenuWidget>> _registeredWidgets;
	// オプションが選択された時のコールバック関数
	CallBackHandler<void, MenuUIActor*> onOptionSelected;

	// ロードされたか
	bool _isLoaded = false;
};

// メニューUIのウィジェット基底クラス
class MenuWidget 
{
public:
	// オプション構造体
	struct Option
	{
		// 表示名
		std::string label{};
		// 選択時コールバック名
		std::string onSelectedCallbackName{};
		// 選択時の遷移先名
		std::string nextWidgetName{};
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
	void AddOption(const std::string& label, 
		const std::string& onSelectedCallbackName = "",
		const std::string& nextWidgetName = "")
	{
		auto& option = _options.emplace_back();
		option.label = label;
		option.onSelectedCallbackName = onSelectedCallbackName;
		option.nextWidgetName = nextWidgetName;
	}
#pragma endregion

#pragma region アクセサ
	std::string GetName() const { return _name; }

	RectTransform& GetRectTransform() { return _rectTransform; }
#pragma endregion

#pragma region ファイル
	// ファイル読み込み
	virtual void LoadFromFile(nlohmann::json* json);
	// ファイル保存
	virtual void SaveToFile(nlohmann::json* json);
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