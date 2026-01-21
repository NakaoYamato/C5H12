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
	// すべてのページを破棄して最初のページに戻る
	void PopToRootPage();
	// すべてのページを破棄して戻る
	void PopAllPages();
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
	virtual bool LoadFromFile();
	// ファイル保存
	virtual bool SaveToFile();
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
	// 選択肢追加
	virtual void AddOption(const std::string& label,
		const std::string& onSelectedCallbackName = "",
		const std::string& nextWidgetName = "");
	// 選択肢インデックス増加
	virtual size_t AddSelectedOptionIndex();
	// 選択肢インデックス減少
	virtual size_t SubSelectedOptionIndex();
	// インデックス範囲制限
	virtual size_t ClampSelectedOptionIndex(size_t index);
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
	bool CanChangeIndex() const { return _canChangeIndex; }

	void SetCanChangeIndex(bool canChange) { _canChangeIndex = canChange; }
#pragma endregion

#pragma region ファイル
	// ファイル読み込み
	void LoadFromFile(nlohmann::json* json);
	// ファイル保存
	void SaveToFile(nlohmann::json* json);

	// ファイル読み込み処理
	virtual void OnLoadFromFile(nlohmann::json* json) {}
	// ファイル保存処理
	virtual void OnSaveToFile(nlohmann::json* json) {}
#pragma endregion

protected:
	// タイトル描画処理
	virtual void RenderTitle(const RenderContext& rc, MenuUIActor* owner);
	// 説明文描画処理
	virtual void RenderDescription(const RenderContext& rc, MenuUIActor* owner);

protected:
    std::string _name;
	RectTransform _rectTransform;

	// タイトル名
	std::string _title{};
	// タイトル名描画位置
	Vector2 _titleLabelOffset{ 50.0f, 10.0f };
	// タイトル名フォントサイズ
	Vector2 _titleFontSize{ 0.5f, 0.5f };
	// タイトル名色
	Vector4 _titleLabelColor = Vector4::White;
	// タイトル背景
	Sprite _titleSprite;

	// 説明文
	std::string _description{};
	// 説明文の画像
	Sprite _descriptionSprite;
	// 説明文描画位置
	Vector2 _descriptionLabelOffset{ 50.0f, 10.0f };
	// 説明文フォントサイズ
	Vector2 _descriptionFontSize{ 0.5f, 0.5f };
	// 説明文色
	Vector4 _descriptionLabelColor = Vector4::White;

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

	// インデックス変更可能フラグ
	bool _canChangeIndex = true;
};