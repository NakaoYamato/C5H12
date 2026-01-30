#include "MenuUIActor.h"

#include "../../Library/Scene/Scene.h"

#include <Mygui.h>

#define DEBUG_MENUUIACTOR 0 // 使用例

// 生成時処理
void MenuUIActor::OnCreate()
{
	UIActor::OnCreate();

	// ウィジェット生成関数登録
	RegisterWidgetCreateFunc<MenuWidget>();
	RegisterWidgetCreateFunc<MenuMatrixWidget>();
	RegisterWidgetCreateFunc<MenuCheckBoxWidget>();

#if DEBUG_MENUUIACTOR // 使用例
	// コールバック登録
	RegisterOptionSelectedCallback("PopPage", [this](MenuUIActor* owner) -> void
		{
			owner->PopPage();
		});

	SetFilePath("./Data/Resource/Actor/TestMenuUI/MenuUI.json");
	LoadFromFile();

	if (!_isLoaded)
	{
		// ウィジェット登録
		std::shared_ptr<MenuWidget> mainMenu = RegisterWidget(std::make_shared<MenuWidget>("MainMenu"));
		std::shared_ptr<MenuWidget> pop001 = RegisterWidget(std::make_shared<MenuWidget>("Pop001"));
		std::shared_ptr<MenuWidget> pop002 = RegisterWidget(std::make_shared<MenuWidget>("Pop002"));
		std::shared_ptr<MenuWidget> pop003 = RegisterWidget(std::make_shared<MenuWidget>("Pop003"));
		std::shared_ptr<MenuWidget> pop004 = RegisterWidget(std::make_shared<MenuWidget>("Pop004"));

		mainMenu->AddOption("Pop001", "", "Pop001");
		mainMenu->AddOption("Pop002", "", "Pop002");
		mainMenu->AddOption("Pop003", "", "Pop003");
		mainMenu->AddOption("Pop004", "", "Pop004");
		pop001->AddOption("pop001-001", "PopPage");
		pop001->AddOption("pop001-002", "PopPage");
		pop002->AddOption("pop002-001", "PopPage");
		pop002->AddOption("pop002-002", "PopPage");
		pop003->AddOption("pop003-001", "PopPage");
		pop003->AddOption("pop003-002", "PopPage");
		pop004->AddOption("pop004-001", "PopPage");
		pop004->AddOption("pop004-002", "PopPage");
	}

	PushPage("MainMenu");
#endif

	_menuNodeEditor = std::make_unique<MenuNodeEditor>();
	_menuNodeEditor->SetLayoutFilePath("./Data/Resource/Actor/TestMenuUI/MenuLayout.json");
	_menuNodeEditor->LoadFromFile();
}

// 更新時処理
void MenuUIActor::OnUpdate(float elapsedTime)
{
	UIActor::OnUpdate(elapsedTime);
	if (auto currentWidget = GetCurrentWidget())
	{
		if (_isInputEnabled)
		{
			if (InputRepeat(_subOptionIndexActionNames))
				currentWidget->SubSelectedOptionIndex();
			if (InputRepeat(_addOptionIndexActionNames))
				currentWidget->AddSelectedOptionIndex();
			// 現在のウィジェットがMenuMatrixWidgetの場合横方向の入力も処理する
			if (auto matrixWidget = dynamic_cast<MenuMatrixWidget*>(currentWidget))
			{
				if (InputRepeat(_subLayerIndexActionNames))
					matrixWidget->SubOptionLayerIndex();
				if (InputRepeat(_addLayerIndexActionNames))
					matrixWidget->AddOptionLayerIndex();
			}

			if (InputTrigger(_selectActionNames))
				currentWidget->SelectOption(this);
			if (InputTrigger(_backActionNames))
				currentWidget->BackOption(this);
		}

		currentWidget->Update(elapsedTime, this);
	}
}

// 3D描画後の描画時処理
void MenuUIActor::OnDelayedRender(const RenderContext& rc)
{
	UIActor::OnDelayedRender(rc);
	if (auto currentWidget = GetCurrentWidget())
	{
		// 直前のウィジェットを描画するか判定
		if (currentWidget->IsDrawPreviousWidget())
		{
			std::stack<std::string> stackNames = _widgetStackNames;
			stackNames.pop();
			while (!stackNames.empty())
			{
				std::string previousWidgetName = stackNames.top();
				auto it = _registeredWidgets.find(previousWidgetName);
				if (it != _registeredWidgets.end())
				{
					it->second->Render(rc, this);
					// 再起処理
					if (it->second->IsDrawPreviousWidget())
					{
						stackNames.pop();
						continue;
					}
				}

				break;
			}
		}

		currentWidget->Render(rc, this);
	}
}

// GUI描画時処理
void MenuUIActor::OnDrawGui()
{
	static ImGuiTabBarFlags tab_bar_flags =
		ImGuiTabBarFlags_AutoSelectNewTabs |
		ImGuiTabBarFlags_Reorderable |
		ImGuiTabBarFlags_FittingPolicyResizeDown;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem(u8"ウィジェット"))
		{
			ImGui::Checkbox(u8"ノードエディター表示", &_isDrawNodeEditor);
			ImGui::Checkbox(u8"入力有効化", &_isInputEnabled);
			ImGui::Separator();

			DrawWidgetGui();

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(u8"ウィジェット入力"))
		{
			ImGui::Text(u8"選択肢インデックス減少アクション名リスト");
			for (auto& name : _subOptionIndexActionNames)
			{
				ImGui::Text(name.c_str());
			}
			ImGui::Separator();
			ImGui::Text(u8"選択肢インデックス増加アクション名リスト");
			for (auto& name : _addOptionIndexActionNames)
			{
				ImGui::Text(name.c_str());
			}
			ImGui::Separator();
			ImGui::Text(u8"レイヤーインデックス減少アクション名リスト");
			for (auto& name : _subLayerIndexActionNames)
			{
				ImGui::Text(name.c_str());
			}
			ImGui::Separator();
			ImGui::Text(u8"レイヤーインデックス増加アクション名リスト");
			for (auto& name : _addLayerIndexActionNames)
			{
				ImGui::Text(name.c_str());
			}
			ImGui::Text(u8"選択アクション名リスト");
			for (auto& name : _selectActionNames)
			{
				ImGui::Text(name.c_str());
			}
			ImGui::Text(u8"戻るアクション名リスト");
			for (auto& name : _backActionNames)
			{
				ImGui::Text(name.c_str());
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(u8"ウィジェット生成関数"))
		{
			for (auto& [className, func] : _widgetCreateFuncMap)
			{
				ImGui::PushID(className.c_str());
				ImGui::Text(className.c_str());
				ImGui::SameLine();
				if (ImGui::Button(u8"生成"))
				{
					RegisterWidget(className, "Test");
				}
				ImGui::PopID();
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	// ウィジェット追加処理
	if (_isAddingWidget)
	{
		if (ImGui::Begin(u8"ウィジェット追加", &_isAddingWidget))
		{
			static std::string newWidgetName = "NewWidget";
			ImGui::InputText(u8"新規ウィジェット名", &newWidgetName);
			if (ImGui::Button(u8"追加"))
			{
				if (_registeredWidgets.find(newWidgetName) == _registeredWidgets.end())
				{
					auto newWidget = std::make_shared<MenuWidget>(newWidgetName);
					RegisterWidget(newWidget);
				}
				_isAddingWidget = false;
			}
		}
		ImGui::End();
	}

	// ノードエディター表示
	if (_isDrawNodeEditor)
	{
		_menuNodeEditor->DrawGui(this, &_isDrawNodeEditor);
	}
}

// ウィジェット登録
std::shared_ptr<MenuWidget> MenuUIActor::RegisterWidget(std::shared_ptr<MenuWidget> widget)
{
	// すでに登録されている場合は名前を変更する
	if (_registeredWidgets.find(widget->GetName()) != _registeredWidgets.end())
	{
		int index = 0;
		while (true)
		{
			std::string name = widget->GetName() + "_" + std::to_string(index++);
			if (_registeredWidgets.find(name) == _registeredWidgets.end())
			{
				// 名前が存在しないなら変更して抜ける
				widget->SetName(widget->GetName() + "_1");
				break;
			}
		}
	}
	_registeredWidgets[widget->GetName()] = widget;
	return widget;
}

// 新しいページをスタックの一番上に積む
void MenuUIActor::PushPage(std::string name)
{
	if (!name.empty())
	{
		if (!_widgetStackNames.empty())
		{
			// 現在のページと同じ場合は何もしない
			if (_widgetStackNames.top() == name)
				return;

			// 現在のページがあれば、終了時の処理
			auto& currentWidget = _registeredWidgets.at(_widgetStackNames.top());
			if (currentWidget) currentWidget->OnExit();
		}

		// 要素があるか確認
		if (_registeredWidgets.find(name) != _registeredWidgets.end())
		{
			auto& widget = _registeredWidgets.at(name);
			if (widget) widget->OnEnter();
			_widgetStackNames.push(name);
		}
	}
}

// 一番上のページを破棄して戻る
void MenuUIActor::PopPage()
{
	if (!_widgetStackNames.empty())
	{
		// 現在のページがあれば、終了時の処理
		if (auto& currentWidget = _registeredWidgets.at(_widgetStackNames.top()))
		{
			currentWidget->OnExit();
		}
		_widgetStackNames.pop();

		// 戻った先のページがあれば、再開時の処理
		if (!_widgetStackNames.empty())
		{
			if (auto& nextWidget = _registeredWidgets.at(_widgetStackNames.top()))
				nextWidget->OnEnter();
		}
	}
}

// すべてのページを破棄して最初のページに戻る
void MenuUIActor::PopToRootPage()
{
	size_t stackSize = _widgetStackNames.size();
	for (size_t i = 0; i < stackSize - 1; ++i)
	{
		PopPage();
	}
}

// すべてのページを破棄して戻る
void MenuUIActor::PopAllPages()
{
	while (!_widgetStackNames.empty())
	{
		PopPage();
	}
}

#pragma region アクセサ
MenuWidget* MenuUIActor::GetCurrentWidget()
{
	if (!_widgetStackNames.empty())
	{
		auto& currentWidget = _registeredWidgets.at(_widgetStackNames.top());
		return currentWidget.get();
	}
	return nullptr;
}
#pragma endregion

#pragma region コールバック
// オプションが選択可能かのコールバック登録
void MenuUIActor::RegisterCanSelectOptionCallback(const std::string& name, CallBack<bool, MenuUIActor*> callback)
{
	_canSelectOption.RegisterCallBack(name, callback);
}
// コールバック登録
void MenuUIActor::RegisterOptionSelectedCallback(const std::string& name, CallBack<void, MenuUIActor*> callback)
{
	_onOptionSelected.RegisterCallBack(name, callback);
}

// オプションが選択可能かのコールバックを呼び出す
bool MenuUIActor::CallCanSelectOption(const std::string& callbackName)
{
	return _canSelectOption.Call(callbackName, this);
}

// オプションが選択された時のコールバックを呼び出す
void MenuUIActor::CallOptionSelected(const std::string& callbackName)
{
	_onOptionSelected.CallVoid(callbackName, this);
}
#pragma endregion

#pragma region ファイル
// ファイル読み込み
bool MenuUIActor::LoadFromFile()
{
	nlohmann::json json;
	if (!Exporter::LoadJsonFile(_filepath.c_str(), &json))
		return false;

	// 各ウィジェット読み込み
	size_t widgetSize = json["widgetSize"].get<size_t>();
	for (size_t index = 0; index < widgetSize; ++index)
	{
		auto& sub = json[std::to_string(index)];
		std::string name = sub.value("name", "");
		std::string widgetName = sub.value("widgetName", ClassToString<MenuWidget>());
		if (name.empty())
			continue;
		// すでに登録されているか確認
		if (_registeredWidgets.find(name) != _registeredWidgets.end())
		{
			// 登録されている場合はそれを使用
			_registeredWidgets[name]->LoadFromFile(&sub);
		}
		else
		{
			// 登録されていない場合は新規作成して登録
			auto widget = RegisterWidget(widgetName, name);
			widget->LoadFromFile(&sub);
		}
	}

	_isLoaded = true;

	return true;
}
// ファイル保存
bool MenuUIActor::SaveToFile()
{
	nlohmann::json json;
	json["widgetSize"] = _registeredWidgets.size();
	// 各ウィジェット保存
	size_t index = 0;
	for (const auto& [name, widget] : _registeredWidgets)
	{
		auto& sub = json[std::to_string(index)];
		sub["name"] = name;
		// クラス名を保存
		sub["widgetName"] = widget->GetWidgetName();
		widget->SaveToFile(&sub);
		index++;
	}

	return Exporter::SaveJsonFile(_filepath.c_str(), json);
}
#pragma endregion

// 入力判定処理
bool MenuUIActor::InputRepeat(const std::vector<std::string>& actionNames)
{
	for (auto& name : actionNames)
	{
		if (_INPUT_REPEAT(name))
			return true;
	}
	return false;
}

// 入力判定処理
bool MenuUIActor::InputTrigger(const std::vector<std::string>& actionNames)
{
	for (auto& name : actionNames)
	{
		if (_INPUT_TRIGGERD(name))
			return true;
	}
	return false;
}

void MenuUIActor::DrawWidgetGui()
{
	static ImGuiTabBarFlags tab_bar_flags =
		ImGuiTabBarFlags_AutoSelectNewTabs |
		ImGuiTabBarFlags_Reorderable |
		ImGuiTabBarFlags_FittingPolicyResizeDown;
	if (ImGui::BeginTabBar("WidgetTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem(u8"現在のページ"))
		{
			if (auto currentWidget = GetCurrentWidget())
			{
				ImGui::Text(_widgetStackNames.top().c_str());
				currentWidget->DrawGui(this);
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(u8"登録ウィジェット一覧"))
		{
			if (ImGui::Button(u8"ウィジェット追加"))
			{
				_isAddingWidget = true;
			}
			ImGui::Separator();

			for (auto& [name, widget] : _registeredWidgets)
			{
				if (ImGui::TreeNode(name.c_str()))
				{
					ImGui::PushID(name.c_str());

					widget->DrawGui(this);
					if (ImGui::Button(u8"ページに移動"))
					{
						PushPage(name);
					}

					ImGui::Separator();
					ImGui::PopID();
					ImGui::TreePop();
				}
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(u8"登録コールバック一覧"))
		{
			auto callbackNames = _onOptionSelected.GetCallBackNames();
			for (const auto& name : callbackNames)
			{
				ImGui::Text(name.c_str());
			}

			ImGui::EndTabItem();
		}

		// ファイル操作
		ImGui::Separator();
		ImGui::InputText(u8"ファイルパス", &_filepath);
		if (ImGui::Button(u8"メニューUI保存"))
		{
			SaveToFile();
		}


		ImGui::EndTabBar();
	}
}

#pragma region メニューUIノードエディタクラス
// 文字列ハッシュ
unsigned long MenuNodeEditor::HashString(const std::string& str) const
{
	unsigned long hash = 5381;
	for (char c : str) {
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}
// ウィジェット名からノードID生成
ne::NodeId MenuNodeEditor::GetNodeId(const std::string& widgetName) const
{
	return (ne::NodeId)HashString("Node_" + widgetName);
}
// ウィジェット名から入力ピンID生成 (左側)
ne::PinId MenuNodeEditor::GetInputPinId(const std::string& widgetName) const
{
	return (ne::PinId)HashString("InPin_" + widgetName);
}
// ウィジェット名とオプションインデックスから出力ピンID生成 (右側)
ne::PinId MenuNodeEditor::GetOutputPinId(const std::string& widgetName, int optionIndex) const
{
	return (ne::PinId)HashString("OutPin_" + widgetName + "_" + std::to_string(optionIndex));
}
// リンクID生成 (遷移元ウィジェット名 + インデックス -> 遷移先ウィジェット名)
ne::LinkId MenuNodeEditor::GetLinkId(const std::string& sourceWidget, int optionIndex, const std::string& targetWidget) const
{
	return (ne::LinkId)HashString("Link_" + sourceWidget + "_" + std::to_string(optionIndex) + "_" + targetWidget);
}

MenuNodeEditor::MenuNodeEditor()
{
	ne::Config config;
	config.SettingsFile = ""; // ImGui側で保存しない（自前でJSON管理するため）
	_editorContext = ne::CreateEditor(&config);
}
MenuNodeEditor::~MenuNodeEditor()
{
	if (_editorContext)
	{
		ne::DestroyEditor(_editorContext);
	}
}

// GUI描画処理
void MenuNodeEditor::DrawGui(MenuUIActor* menuActor, bool* flag)
{
	if (!menuActor || !_editorContext) return;

	ne::SetCurrentEditor(_editorContext);

	// ウィンドウ開始
	ImGui::Begin("Menu Node Editor", flag, ImGuiWindowFlags_MenuBar);

	// メニューバー
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Fit View")) ne::NavigateToContent();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save Layout")) SaveToFile();
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	
	// カラムで画面を分割 (左: エディタ, 右: インスペクタ)
	ImGui::Columns(2, "NodeEditorColumns", true);

	ne::Begin("MenuNodeEditorSpace");

	// ノードとピンの描画
	auto& widgets = menuActor->GetRegisteredWidgetsForEdit();

	// 逆引き用マップ (PinID -> {WidgetName, OptionIndex})
	// リンク作成時のコールバックで使用
	struct PinInfo { std::string widgetName{}; int optionIndex{}; bool isInput{}; };
	std::unordered_map<size_t, PinInfo> pinLookup;

	int nodeCount = 0;
	for (auto& [name, widget] : widgets)
	{
		ne::NodeId nodeId = GetNodeId(name);

		// 初回位置設定
		if (_isFirstFrame)
		{
			if (_nodePositions.find((size_t)nodeId) != _nodePositions.end())
			{
				ne::SetNodePosition(nodeId, _nodePositions[(size_t)nodeId]);
			}
			else
			{
				// デフォルト配置: 数が増えると重なるので適当にずらす
				ne::SetNodePosition(nodeId, ImVec2(50.0f + (nodeCount % 5) * 300.0f, 50.0f + (nodeCount / 5) * 200.0f));
			}
		}
		else
		{
			// 位置を記録
			_nodePositions[(size_t)nodeId] = ne::GetNodePosition(nodeId);
		}

		// 色設定
		bool isCurrent = (menuActor->GetCurrentWidget() == widget.get());
		bool isSelectedInEditor = (name == _selectedWidgetName); // エディタでの選択状態
		ImVec4 headerColor = (name == "MainMenu") ? _rootNodeColor : _nodeBgColor;
		float borderWidth = 1.0f;

		if (isCurrent)
		{
			// 現在のアクティブページ
			headerColor = ImVec4(0.8f, 0.4f, 0.0f, 1.0f);
			borderWidth = 3.0f;
		}
		else if (isSelectedInEditor)
		{
			// エディタで選択中
			headerColor = ImVec4(0.5f, 1.0f, 0.5f, 1.0f);
			borderWidth = 2.0f;
		}

		ne::PushStyleColor(ne::StyleColor_NodeBg, _nodeBgColor);
		ne::PushStyleColor(ne::StyleColor_NodeBorder, isCurrent ? ImVec4(1, 1, 0, 1) : _nodeBorderColor);
		ne::PushStyleVar(ne::StyleVar_NodeBorderWidth, borderWidth);

		ne::BeginNode(nodeId);

		// ヘッダー
		ImGui::BeginGroup();

		// 入力ピン (左上)
		ne::PinId inPinId = GetInputPinId(name);
		ne::BeginPin(inPinId, ne::PinKind::Input);
		// 入力ピンの見た目
		ImGui::Text(" > ");
		ne::EndPin();
		pinLookup[(size_t)inPinId] = { name, -1, true };

		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", name.c_str());
		ImGui::EndGroup();

		auto& options = widget->GetOptions();
		for (int i = 0; i < options.size(); ++i)
		{
			auto& option = options[i];

			ne::PinId outPinId = GetOutputPinId(name, i);

			// ラベル表示
			ImGui::Text("%d: %s", i, option.label.c_str());
			if (!option.onSelectedCallbackName.empty())
			{
				ImGui::SameLine();
				ImGui::TextDisabled("(cb: %s)", option.onSelectedCallbackName.c_str());
			}

			ImGui::SameLine();

			// 出力ピン (右側)
			ne::BeginPin(outPinId, ne::PinKind::Output);
			ImGui::Text(" >");
			ne::EndPin();

			pinLookup[(size_t)outPinId] = { name, i, false };
		}

		ne::EndNode();

		ne::PopStyleVar(1);
		ne::PopStyleColor(2);

		nodeCount++;
	}

	_isFirstFrame = false;

	// リンクの描画
	for (auto& [name, widget] : widgets)
	{
		auto& options = widget->GetOptions();
		for (int i = 0; i < options.size(); ++i)
		{
			auto& option = options[i];
			if (!option.nextWidgetName.empty() && widgets.find(option.nextWidgetName) != widgets.end())
			{
				// 遷移先が存在する場合、リンクを描画
				if (widgets.find(option.nextWidgetName) != widgets.end())
				{
					ne::LinkId linkId = GetLinkId(name, i, option.nextWidgetName);
					ne::PinId startPin = GetOutputPinId(name, i);
					ne::PinId endPin = GetInputPinId(option.nextWidgetName);

					ne::Link(linkId, startPin, endPin);
				}
			}
		}
	}

	// リンクの作成・削除処理
	if (ne::BeginCreate())
	{
		ne::PinId startPinId, endPinId;
		if (ne::QueryNewLink(&startPinId, &endPinId))
		{
			if (startPinId && endPinId && pinLookup.count((size_t)startPinId) && pinLookup.count((size_t)endPinId))
			{
				auto& startInfo = pinLookup[(size_t)startPinId];
				auto& endInfo = pinLookup[(size_t)endPinId];

				// Input同士、Output同士でないか確認
				if (startInfo.isInput != endInfo.isInput)
				{
					if (ne::AcceptNewItem())
					{
						auto* outInfo = startInfo.isInput ? &endInfo : &startInfo;
						auto* inInfo = startInfo.isInput ? &startInfo : &endInfo;
						auto& options = widgets[outInfo->widgetName]->GetOptions();
						if (outInfo->optionIndex >= 0 && outInfo->optionIndex < options.size())
							options[outInfo->optionIndex].nextWidgetName = inInfo->widgetName;
					}
				}
				else
				{
					ne::RejectNewItem(ImVec4(1, 0, 0, 1), 2.0f);
				}
			}
		}
	}
	ne::EndCreate();

	// リンク削除
	if (ne::BeginDelete())
	{
		ne::LinkId deletedLinkId;
		while (ne::QueryDeletedLink(&deletedLinkId))
		{
			if (ne::AcceptDeletedItem())
			{
				// リンクIDからどのウィジェットのどのオプションかを逆算するのは難しいので、
				// 全探索して該当するリンクを探して削除する
				bool found = false;
				for (auto& [name, widget] : widgets)
				{
					auto& options = widget->GetOptions();
					for (int i = 0; i < options.size(); ++i)
					{
						if (options[i].nextWidgetName.empty()) continue;

						ne::LinkId currentLinkId = GetLinkId(name, i, options[i].nextWidgetName);
						if (currentLinkId == deletedLinkId)
						{
							// リンク解除
							options[i].nextWidgetName = "";
							found = true;
							break;
						}
					}
					if (found) break;
				}
			}
		}
	}
	ne::EndDelete();

	// コンテキストメニュー
	ne::Suspend();
	// リンク(線)の右クリックメニュー
	if (ne::ShowLinkContextMenu(&_contextLinkId))
	{
		ImGui::OpenPopup("LinkContextMenu");
	}

	if (ImGui::BeginPopup("LinkContextMenu"))
	{
		if (ImGui::MenuItem("Delete Link"))
		{
			// 全ウィジェットを探索して、右クリックされたリンクIDに該当する接続を探す
			bool found = false;
			for (auto& [name, widget] : widgets)
			{
				auto& options = widget->GetOptions();
				for (int i = 0; i < options.size(); ++i)
				{
					// 遷移先がないオプションはスキップ
					if (options[i].nextWidgetName.empty()) continue;

					// リンクIDを再計算して、右クリックされたIDと照合
					ne::LinkId currentLinkId = GetLinkId(name, i, options[i].nextWidgetName);
					if (currentLinkId == _contextLinkId)
					{
						options[i].nextWidgetName = ""; // 接続解除
						found = true;
						break;
					}
				}
				if (found) break;
			}
		}
		ImGui::EndPopup();
	}

	if (ne::ShowBackgroundContextMenu())
	{
		ImGui::OpenPopup("CreateWidgetContext");
		_popupSpawnPos = ImGui::GetMousePos();
	}

	// 新規ウィジェット作成ポップアップ
	if (ImGui::BeginPopup("CreateWidgetContext"))
	{
		ImGui::Text("Create New Widget");
		ImGui::InputText("Name", &_newWidgetName);
		// 作成するウィジェットのクラス選択
		if (ImGui::BeginCombo(u8"ウィジェットのクラス", _newWidgetClassName.c_str()))
		{
			for (auto& [className, func] : menuActor->GetWidgetCreateFuncMap())
			{
				if (ImGui::Selectable(className.c_str(), _newWidgetClassName == className))
				{
					_newWidgetClassName = className;
				}
			}
			ImGui::EndCombo();
		}

		if (ImGui::Button("Create"))
		{
			auto newWidget = menuActor->RegisterWidget(_newWidgetClassName, _newWidgetName);
			if (newWidget)
			{
				// ウィジェット作成成功
				// ウィジェットが重複していたら名前が変わる可能性があるので、再取得
				_newWidgetName = newWidget->GetName();
				ne::SetNodePosition(GetNodeId(_newWidgetName), ImVec2(50, 50));
			}

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ne::Resume();

	// 選択状態の更新
	if (ne::GetSelectedObjectCount() > 0)
	{
		std::vector<ne::NodeId> selectedNodes(1);
		ne::GetSelectedNodes(selectedNodes.data(), 1);
		ne::NodeId selectedId = selectedNodes[0];

		// IDから名前を逆引き
		bool found = false;
		for (auto& [name, widget] : widgets)
		{
			if (GetNodeId(name) == selectedId)
			{
				_selectedWidgetName = name;
				found = true;
				break;
			}
		}
		if (!found) _selectedWidgetName.clear();
	}
	else
	{
		// 何も選択されていない場合
		_selectedWidgetName.clear();
	}

	ne::End();

	// インスペクタ
	ImGui::NextColumn();

	ImGui::Text("Inspector");
	ImGui::Separator();

	if (!_selectedWidgetName.empty())
	{
		auto it = widgets.find(_selectedWidgetName);
		if (it != widgets.end())
		{
			ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "[ %s ]", _selectedWidgetName.c_str());
			it->second->DrawGui(menuActor);
			ImGui::Separator();
			if (ImGui::Button(u8"ページに移動"))
			{
				menuActor->PushPage(_selectedWidgetName);
			}
		}
	}
	else
	{
		ImGui::TextDisabled("Select a node to edit.");
	}

	ImGui::Columns(1);
	ImGui::End();
}

// ファイル読み込み
void MenuNodeEditor::LoadFromFile()
{
	if (_filename.empty()) return;

	nlohmann::json jsonData;
	if (Exporter::LoadJsonFile(_filename.c_str(), &jsonData))
	{
		if (jsonData.contains("Nodes"))
		{
			auto& nodes = jsonData["Nodes"];
			for (auto& element : nodes)
			{
				size_t nodeId = element["ID"].get<size_t>();
				float x = element["X"].get<float>();
				float y = element["Y"].get<float>();

				_nodePositions[nodeId] = ImVec2(x, y);
			}
		}
	}
	// 初回フレームフラグをリセットして、再適用させる
	_isFirstFrame = true;
}
// ファイル保存
void MenuNodeEditor::SaveToFile()
{
	if (_filename.empty()) return;

	nlohmann::json jsonData;
	nlohmann::json nodesArray = nlohmann::json::array();

	// 現在のエディタ上の全ノードの位置を保存
	for (auto& [id, pos] : _nodePositions)
	{
		// 最新の位置情報を取得して更新
		ImVec2 currentPos = ne::GetNodePosition((ne::NodeId)id);
		_nodePositions[id] = currentPos;

		nlohmann::json node;
		node["ID"] = id;
		node["X"] = currentPos.x;
		node["Y"] = currentPos.y;
		nodesArray.push_back(node);
	}
	jsonData["Nodes"] = nodesArray;

	Exporter::SaveJsonFile(_filename.c_str(), jsonData);
}

#pragma endregion
