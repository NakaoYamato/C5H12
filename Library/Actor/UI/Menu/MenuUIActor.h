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
	std::shared_ptr<MenuWidget> RegisterWidget(std::shared_ptr<MenuWidget> widget);
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

	void SetFilePath(const std::string& filepath) { _filepath = filepath; }
	// ロード済みか
	bool IsLoaded() const { return _isLoaded; }
	// ウィジェット追加中か
	bool IsAddingWidget() const { return _isAddingWidget; }
#pragma endregion

#pragma region ファイル
	// ファイル読み込み
	virtual void LoadFromFile();
	// ファイル保存
	virtual void SaveToFile();
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

	std::string _filepath{};
	// ロードされたか
	bool _isLoaded = false;
	// GUIでのウィジェット追加用フラグ
	bool _isAddingWidget = false;
};

// メニューUIのウィジェット基底クラス
class MenuWidget 
{
public:
	// 選択肢
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

#pragma region 選択肢
	void AddOption(const std::string& label,
		const std::string& onSelectedCallbackName = "",
		const std::string& nextWidgetName = "");
	// 選択肢インデックス増加
	virtual size_t AddSelectedOptionIndex();
	// 選択肢インデックス減少
	virtual size_t SubSelectedOptionIndex();
	// 選択肢選択処理
	virtual void SelectOption(MenuUIActor* owner);
	// 戻る選択肢選択処理
	virtual void BackOption(MenuUIActor* owner);
#pragma endregion

#pragma region アクセサ
	std::string GetName() const { return _name; }

	RectTransform& GetRectTransform() { return _rectTransform; }
	size_t GetOptionSize() const { return _options.size(); }
	size_t GetSelectedOptionIndex() const { return _selectedOptionIndex; }
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

	// 選択肢リスト
	std::vector<Option> _options;
	// 選択肢の背景画像
	Sprite _optionSprite;
	// 選択中の選択肢画像
	Sprite _selectedOptionSprite;
	// 選択肢間の垂直間隔
	float _optionVerticalSpacing = 40.0f;
	// ラベルのフォントサイズ
	Vector2 _optionFontSize = Vector2::One;
	// ラベルのオフセット
	Vector2 _optionLabelOffset = Vector2(20.0f, 10.0f);
	// ラベルの色
	Vector4 _optionLabelColor = Vector4::White;
	// ラベルの選択中の色
	Vector4 _optionLabelSelectedColor = Vector4::Red;

	// 選択中の選択肢インデックス
	size_t _selectedOptionIndex = 0;
};