#pragma once

#include <stack>

#include "MenuWidget.h"

#include "../UIActor.h"

#include <imgui.h>
#include <imgui_node_editor.h>

class MenuNodeEditor;

// メニューUIアクタークラス
class MenuUIActor : public UIActor
{
public:
	// ウィジェット生成関数型
	using WidgetCreateFunc = std::function<std::shared_ptr<MenuWidget>(const std::string&)>;

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
	// ウィジェット登録
	std::shared_ptr<MenuWidget> RegisterWidget(const std::string& className, const std::string& name)
	{
		auto it = _widgetCreateFuncMap.find(className);
		if (it == _widgetCreateFuncMap.end())
			return nullptr;

		std::shared_ptr<MenuWidget> widget = it->second(name);
		return RegisterWidget(widget);
	}
	// ウィジェット登録
	template<class T>
	std::shared_ptr<T> RegisterWidget(const std::string& name)
	{
		std::string className = ClassToString<T>();
		auto it = _widgetCreateFuncMap.find(className);
		if (it == _widgetCreateFuncMap.end())
			return nullptr;
		std::shared_ptr<MenuWidget> widget = it->second(name);
		std::shared_ptr<T> castedWidget = std::dynamic_pointer_cast<T>(widget);
		if (castedWidget == nullptr)
			return nullptr;
		RegisterWidget(castedWidget);
		return castedWidget;
	}
	// 新しいページをスタックの一番上に積む
	void PushPage(std::string name);
	// 一番上のページを破棄して戻る
	void PopPage();
	// すべてのページを破棄して最初のページに戻る
	void PopToRootPage();
	// すべてのページを破棄して戻る
	void PopAllPages();

	// ウィジェット生成関数登録
	template<class T>
	void RegisterWidgetCreateFunc()
	{
		std::string className = ClassToString<T>();
		_widgetCreateFuncMap[className] = [&](const std::string& name) -> std::shared_ptr<MenuWidget>
			{
				auto widget = std::make_shared<T>(name);
				return widget;
			};
	}

#pragma region アクセサ
	// 現在のウィジェットを取得
	MenuWidget* GetCurrentWidget();
	// スタック取得
	std::stack<std::string> GetWidgetStack() const { return _widgetStackNames; }
	// 登録ウィジェット取得
	const std::unordered_map<std::string, std::shared_ptr<MenuWidget>>& GetRegisteredWidgets() const { return _registeredWidgets; }
	// ファイルパス取得
	const std::string& GetFilePath() const { return _filepath; }
	// ウィジェット生成関数マップ取得
	const std::unordered_map<std::string, WidgetCreateFunc>& GetWidgetCreateFuncMap() const { return _widgetCreateFuncMap; }

	void SetFilePath(const std::string& filepath) { _filepath = filepath; }
	// ロード済みか
	bool IsLoaded() const { return _isLoaded; }
	// ウィジェット追加中か
	bool IsAddingWidget() const { return _isAddingWidget; }

	bool IsInputEnabled() const { return _isInputEnabled; }
	void SetInputEnabled(bool enabled) { _isInputEnabled = enabled; }

	void SetSubOptionIndexActionNames(const std::vector<std::string>& actionNames) { _subOptionIndexActionNames = actionNames; }
	void SetAddOptionIndexActionNames(const std::vector<std::string>& actionNames) { _addOptionIndexActionNames = actionNames; }
	void SetSubLayerIndexActionNames(const std::vector<std::string>& actionNames) { _subLayerIndexActionNames = actionNames; }
	void SetAddLayerIndexActionNames(const std::vector<std::string>& actionNames) { _addLayerIndexActionNames = actionNames; }
	void SetSelectActionNames(const std::vector<std::string>& actionNames) { _selectActionNames = actionNames; }
	void SetBackActionNames(const std::vector<std::string>& actionNames) { _backActionNames = actionNames; }
#pragma endregion

#pragma region コールバック
	// オプションが選択可能かのコールバック登録
	void RegisterCanSelectOptionCallback(const std::string& name, CallBack<bool, MenuUIActor*> callback);
	// オプションが選択された時のコールバック登録
	void RegisterOptionSelectedCallback(const std::string& name, CallBack<void, MenuUIActor*> callback);

	// オプションが選択可能かのコールバックを呼び出す
	bool CallCanSelectOption(const std::string& callbackName);
	// オプションが選択された時のコールバックを呼び出す
	void CallOptionSelected(const std::string& callbackName);

	// コールバックハンドラ取得
	CallBackHandler<bool, MenuUIActor*>& GetCanSelectOptionCallbackHandler() { return _canSelectOption; }
	// コールバックハンドラ取得
	CallBackHandler<void, MenuUIActor*>& GetOptionSelectedCallbackHandler() { return _onOptionSelected; }
#pragma endregion

#pragma region ファイル
	// ファイル読み込み
	virtual bool LoadFromFile();
	// ファイル保存
	virtual bool SaveToFile();
#pragma endregion

#pragma region デバッグ用
	std::unordered_map<std::string, std::shared_ptr<MenuWidget>>& GetRegisteredWidgetsForEdit() { return _registeredWidgets; }

	bool IsDrawNodeEditor() const { return _isDrawNodeEditor; }
	void SetDrawNodeEditor(bool draw) { _isDrawNodeEditor = draw; }
#pragma endregion

protected:
	// 入力判定処理
	bool InputRepeat(const std::vector<std::string>& actionNames);
	// 入力判定処理
	bool InputTrigger(const std::vector<std::string>& actionNames);

private:
	// GUI描画処理
	void DrawWidgetGui();

private:
	// ウィジェットスタック
	std::stack<std::string> _widgetStackNames;

	// 登録しているウィジェット
	std::unordered_map<std::string, std::shared_ptr<MenuWidget>> _registeredWidgets;
	// ウィジェット生成関数マップ
	std::unordered_map<std::string, WidgetCreateFunc> _widgetCreateFuncMap;

#pragma region コールバック
	// オプションが選択可能かのコールバック関数
	CallBackHandler<bool, MenuUIActor*> _canSelectOption;
	// オプションが選択された時のコールバック関数
	CallBackHandler<void, MenuUIActor*> _onOptionSelected;
#pragma endregion

	std::string _filepath{};
	// ロードされたか
	bool _isLoaded = false;

#pragma region 入力処理
	// 入力処理を行うか
	bool _isInputEnabled = true;

	// 入力アクション名リスト
	std::vector<std::string> _subOptionIndexActionNames{ "Up" };
	std::vector<std::string> _addOptionIndexActionNames{ "Down" };
	std::vector<std::string> _subLayerIndexActionNames{ "Left" };
	std::vector<std::string> _addLayerIndexActionNames{ "Right" };
	std::vector<std::string> _selectActionNames{ "Select" };
	std::vector<std::string> _backActionNames{ "Back", "Menu" };
#pragma endregion

#pragma region デバッグ用
	std::unique_ptr<MenuNodeEditor> _menuNodeEditor;
	// ノードエディター表示フラグ
	bool _isDrawNodeEditor = false;
	// GUIでのウィジェット追加用フラグ
	bool _isAddingWidget = false;
#pragma endregion
};

// メニューUIノードエディタクラス
namespace ne = ax::NodeEditor;
class MenuNodeEditor
{
public:
    MenuNodeEditor();
    ~MenuNodeEditor();

	// GUI描画処理
	void DrawGui(MenuUIActor* menuActor, bool* flag = nullptr);

	// ファイルパス設定
	void SetLayoutFilePath(const std::string& filename) { _filename = filename; }
	// ファイル読み込み
	void LoadFromFile();
	// ファイル保存
	void SaveToFile();

private:
#pragma region ヘルパー
	// 文字列ハッシュ
	unsigned long HashString(const std::string& str) const;
	// ウィジェット名からノードID生成
	ne::NodeId GetNodeId(const std::string& widgetName) const;
	// ウィジェット名から入力ピンID生成 (左側)
	ne::PinId GetInputPinId(const std::string& widgetName) const;
	// ウィジェット名とオプションインデックスから出力ピンID生成 (右側)
	ne::PinId GetOutputPinId(const std::string& widgetName, int optionIndex) const;
	// リンクID生成 (遷移元ウィジェット名 + インデックス -> 遷移先ウィジェット名)
	ne::LinkId GetLinkId(const std::string& sourceWidget, int optionIndex, const std::string& targetWidget) const;
#pragma endregion

private:
	ne::EditorContext* _editorContext = nullptr;
	std::string _filename = "";

	// ノード位置のキャッシュ (NodeId -> ImVec2)
	std::unordered_map<size_t, ImVec2> _nodePositions;
	bool _isFirstFrame = true;

	// コンテキストメニュー用リンクID
	ne::LinkId _contextLinkId = 0;
	// 新規ウィジェット名
	std::string _newWidgetName = "NewWidget";
	// 選択中のウィジェット名
	std::string _selectedWidgetName{};
	// ポップアップ出現位置
	ImVec2 _popupSpawnPos{};
	// 生成するウィジェットクラス名
	std::string _newWidgetClassName = "MenuWidget";

	// ノードのスタイル設定
	const float PIN_WIDTH = 24.0f;
	const float NODE_WIDTH = 200.0f;

	// 色設定
	const ImVec4 _nodeBgColor = ImVec4(40 / 255.0f, 40 / 255.0f, 45 / 255.0f, 230 / 255.0f);
	const ImVec4 _nodeBorderColor = ImVec4(100 / 255.0f, 100 / 255.0f, 100 / 255.0f, 255 / 255.0f);
	const ImVec4 _rootNodeColor = ImVec4(200 / 255.0f, 50 / 255.0f, 50 / 255.0f, 230 / 255.0f); // MainMenuなどを強調

};