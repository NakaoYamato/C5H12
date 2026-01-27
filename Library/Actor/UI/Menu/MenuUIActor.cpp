#include "MenuUIActor.h"

#include "../../Library/Scene/Scene.h"

#include <Mygui.h>

#define DEBUG_MENUUIACTOR 0 // 使用例

// コピー用ポインタ
MenuWidget* CopyMenuWidgetPtr = nullptr;

// 生成時処理
void MenuUIActor::OnCreate()
{
	UIActor::OnCreate();

	// ウィジェット生成関数登録
	RegisterWidgetCreateFunc<MenuWidget>();
	RegisterWidgetCreateFunc<MenuMatrixWidget>();

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
			// スタックから一時的に直前のウィジェットを取得して描画する
			std::string currentWidgetName = _widgetStackNames.top();
			_widgetStackNames.pop();
			if (!_widgetStackNames.empty())
			{
				std::string previousWidgetName = _widgetStackNames.top();
				auto it = _registeredWidgets.find(previousWidgetName);
				if (it != _registeredWidgets.end())
				{
					it->second->Render(rc, this);
				}
			}
			_widgetStackNames.push(currentWidgetName);
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

// コールバック登録
void MenuUIActor::RegisterOptionSelectedCallback(const std::string& name, CallBack<void, MenuUIActor*> callback)
{
	onOptionSelected.RegisterCallBack(name, callback);
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

// オプションが選択された時のコールバックを呼び出す
void MenuUIActor::CallOptionSelected(const std::string& callbackName)
{
	onOptionSelected.CallVoid(callbackName, this);
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
			auto callbackNames = onOptionSelected.GetCallBackNames();
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

#pragma region MenuWidget
// 更新処理
void MenuWidget::Update(float elapsedTime, MenuUIActor* owner)
{
	// トランスフォーム更新
	_rectTransform.UpdateTransform(&owner->GetRectTransform());
	_titleSprite.UpdateTransform(&_rectTransform);
	_descriptionSprite.UpdateTransform(&_rectTransform);
}
// 描画処理
void MenuWidget::Render(const RenderContext& rc, MenuUIActor* owner)
{
	// タイトル描画
	RenderTitle(rc, owner);

	// 説明文描画
	RenderDescription(rc, owner);

	// 選択肢描画
	RenderOptions(rc, owner);
}
// GUI描画処理
void MenuWidget::DrawGui(MenuUIActor* owner)
{
	if (ImGui::Button(u8"コピー"))
	{
		CopyMenuWidgetPtr = this;
	}
	if (CopyMenuWidgetPtr)
	{
		ImGui::SameLine();
		if (ImGui::Button(u8"ペースト"))
		{
			this->_rectTransform = CopyMenuWidgetPtr->_rectTransform;
			this->_title = CopyMenuWidgetPtr->_title;
			this->_titleLabelOffset = CopyMenuWidgetPtr->_titleLabelOffset;
			this->_titleFontSize = CopyMenuWidgetPtr->_titleFontSize;
			this->_titleLabelColor = CopyMenuWidgetPtr->_titleLabelColor;
			this->_titleSprite = CopyMenuWidgetPtr->_titleSprite;
			this->_description = CopyMenuWidgetPtr->_description;
			this->_descriptionLabelOffset = CopyMenuWidgetPtr->_descriptionLabelOffset;
			this->_descriptionFontSize = CopyMenuWidgetPtr->_descriptionFontSize;
			this->_descriptionLabelColor = CopyMenuWidgetPtr->_descriptionLabelColor;
			this->_descriptionSprite = CopyMenuWidgetPtr->_descriptionSprite;
			this->_options = CopyMenuWidgetPtr->_options;
			this->_optionSprite = CopyMenuWidgetPtr->_optionSprite;
			this->_selectedOptionSprite = CopyMenuWidgetPtr->_selectedOptionSprite;
			this->_optionVerticalSpacing = CopyMenuWidgetPtr->_optionVerticalSpacing;
			this->_optionLabelFontSize = CopyMenuWidgetPtr->_optionLabelFontSize;
			this->_optionLabelOffset = CopyMenuWidgetPtr->_optionLabelOffset;
			this->_optionLabelColor = CopyMenuWidgetPtr->_optionLabelColor;
			this->_optionLabelSelectedColor = CopyMenuWidgetPtr->_optionLabelSelectedColor;
			this->_isDrawPreviousWidget = CopyMenuWidgetPtr->_isDrawPreviousWidget;
		}
	}
	ImGui::Separator();

	if (ImGui::TreeNode(u8"トランスフォーム"))
	{
		_rectTransform.DrawGui();
		ImGui::TreePop();
	}
	ImGui::Separator();

	if (ImGui::TreeNode(u8"画像"))
	{
		DrawSpritesGui(owner);
		ImGui::TreePop();
	}
	ImGui::Separator();

	if (ImGui::TreeNode(u8"選択肢"))
	{
		static std::string newOptionLabel = "NewOption";
		ImGui::InputText(u8"新規選択肢", &newOptionLabel);
		if (ImGui::Button(u8"選択肢追加"))
		{
			AddOption(newOptionLabel);
		}

		ImGui::Separator();

		for (size_t i = 0; i < _options.size(); ++i)
		{
			ImGui::PushID(static_cast<int>(i));

			auto& option = _options[i];
			ImGui::InputText(u8"ラベル", &option.label);
			if (ImGui::TreeNode(u8"選択時コールバック"))
			{
				if (ImGui::RadioButton(u8"なし", option.onSelectedCallbackName.empty()))
				{
					option.onSelectedCallbackName.clear();
				}
				for (auto& str : owner->GetOptionSelectedCallbackHandler().GetCallBackNames())
				{
					if (ImGui::RadioButton(str.c_str(), option.onSelectedCallbackName == str))
					{
						option.onSelectedCallbackName = str;
					}
				}

				ImGui::TreePop();
			}
			if (ImGui::TreeNode(u8"選択時遷移先"))
			{
				if (ImGui::RadioButton(u8"なし", option.nextWidgetName.empty()))
				{
					option.nextWidgetName.clear();
				}
				for (auto& [name, widget] : owner->GetRegisteredWidgets())
				{
					if (ImGui::RadioButton(name.c_str(), option.nextWidgetName == name))
					{
						option.nextWidgetName = name;
					}
				}
				ImGui::TreePop();
			}
			if (ImGui::Button(u8"選択肢削除"))
			{
				RemoveOption(i);
				ImGui::PopID();
				break;
			}

			ImGui::PopID();
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode(u8"パラメータ"))
	{
		ImGui::DragFloat(u8"選択肢垂直間隔", &_optionVerticalSpacing);
		ImGui::DragFloat2(u8"ラベルフォントサイズ", &_optionLabelFontSize.x);
		ImGui::DragFloat2(u8"ラベルオフセット", &_optionLabelOffset.x);
		ImGui::ColorEdit4(u8"ラベル色", &_optionLabelColor.x);
		ImGui::ColorEdit4(u8"選択中ラベル色", &_optionLabelSelectedColor.x);

		ImGui::Text(u8"選択中選択肢インデックス: %d", static_cast<int>(_selectedOptionIndex));

		ImGui::Checkbox(u8"インデックス変更可能", &_canChangeIndex);

		ImGui::Checkbox(u8"以前のウィジェットを描画", &_isDrawPreviousWidget);
		
		ImGui::TreePop();
	}

	if (ImGui::TreeNode(u8"タイトル"))
	{
		ImGui::InputText(u8"タイトルテキスト", &_title);
		ImGui::DragFloat2(u8"タイトルオフセット", &_titleLabelOffset.x);
		ImGui::DragFloat2(u8"タイトルフォントサイズ", &_titleFontSize.x);
		ImGui::ColorEdit4(u8"タイトル色", &_titleLabelColor.x);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode(u8"説明文"))
	{
		ImGui::InputText(u8"説明文テキスト", &_description);
		ImGui::DragFloat2(u8"説明文オフセット", &_descriptionLabelOffset.x);
		ImGui::DragFloat2(u8"説明文フォントサイズ", &_descriptionFontSize.x);
		ImGui::ColorEdit4(u8"説明文色", &_descriptionLabelColor.x);

		ImGui::TreePop();
	}
}

#pragma region 選択肢
// 選択肢追加
void MenuWidget::AddOption(const std::string& label, const std::string& onSelectedCallbackName, const std::string& nextWidgetName)
{
	auto& option = _options.emplace_back();
	option.label = label;
	option.onSelectedCallbackName = onSelectedCallbackName;
	option.nextWidgetName = nextWidgetName;
}
// 選択肢削除
void MenuWidget::RemoveOption(size_t index)
{
	if (index < _options.size())
	{
		_options.erase(_options.begin() + index);
	}
}
// 選択肢インデックス増加
int MenuWidget::AddSelectedOptionIndex()
{
	if (!_canChangeIndex)
		return _selectedOptionIndex;

	_selectedOptionIndex++;
	// インデックス範囲制限
	_selectedOptionIndex = ClampSelectedOptionIndex(_selectedOptionIndex);
	return _selectedOptionIndex;
}
// 選択肢インデックス減少
int MenuWidget::SubSelectedOptionIndex()
{
	if (!_canChangeIndex)
		return _selectedOptionIndex;

	_selectedOptionIndex--;
	// インデックス範囲制限
	_selectedOptionIndex = ClampSelectedOptionIndex(_selectedOptionIndex);
	return _selectedOptionIndex;
}
// インデックス範囲制限
int MenuWidget::ClampSelectedOptionIndex(int index)
{
	if (_options.empty())
		return 0;
	index = index % (static_cast<int>(_options.size()));
	if (index < 0)
		index += static_cast<int>(_options.size());
	return index;
}
// 選択肢選択処理
void MenuWidget::SelectOption(MenuUIActor* owner)
{
	if (_options.empty())
		return;

	auto& option = _options[_selectedOptionIndex];
	if (!option.onSelectedCallbackName.empty())
	{
		owner->CallOptionSelected(option.onSelectedCallbackName);
	}
	if (!option.nextWidgetName.empty())
	{
		owner->PushPage(option.nextWidgetName);
	}
}
// 戻る選択肢選択処理
void MenuWidget::BackOption(MenuUIActor* owner)
{
	owner->PopPage();
}
#pragma endregion

#pragma region ファイル
// ファイル読み込み
void MenuWidget::LoadFromFile(nlohmann::json* json)
{
	auto& sub = (*json);

	_rectTransform = sub.value("rectTransform", RectTransform());
	_options.clear();
	size_t optionsSize = sub.value("optionsSize", 0);
	for (size_t i = 0; i < optionsSize; ++i)
	{
		std::string label = "option_" + std::to_string(i);
		auto& optionSub = sub[label];
		Option option;
		option.label = optionSub.value("label", "");
		option.onSelectedCallbackName = optionSub.value("onSelectedCallbackName", "");
		option.nextWidgetName = optionSub.value("nextWidgetName", "");
		_options.push_back(option);
	}

	_titleSprite.LoadFromFile(&sub, "titleSprite");
	_descriptionSprite.LoadFromFile(&sub, "descriptionSprite");
	_optionSprite.LoadFromFile(&sub, "optionSprite");
	_selectedOptionSprite.LoadFromFile(&sub, "selectedOptionSprite");

	_title = sub.value("title", "");
	_titleLabelOffset = sub.value("titleLabelOffset", Vector2(50.0f, 10.0f));
	_titleFontSize = sub.value("titleFontSize", Vector2(0.5f, 0.5f));
	_titleLabelColor = sub.value("titleLabelColor", Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	_description = sub.value("description", "");
	_descriptionLabelOffset = sub.value("descriptionLabelOffset", Vector2(50.0f, 10.0f));
	_descriptionFontSize = sub.value("descriptionFontSize", Vector2(0.5f, 0.5f));
	_descriptionLabelColor = sub.value("descriptionLabelColor", Vector4(1.0f, 1.0f, 1.0f, 1.0f));


	_optionVerticalSpacing = sub.value("optionVerticalSpacing", 40.0f);
	_optionLabelFontSize = sub.value("optionFontSize", Vector2(1.0f, 1.0f));
	_optionLabelOffset = sub.value("optionLabelOffset", Vector2(20.0f, 10.0f));
	_optionLabelColor = sub.value("optionLabelColor", Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	_optionLabelSelectedColor = sub.value("optionLabelSelectedColor", Vector4(1.0f, 0.0f, 0.0f, 1.0f));

	_isDrawPreviousWidget = sub.value("isDrawPreviousWidget", false);

	// ファイル読み込み処理
	OnLoadFromFile(json);
}
// ファイル保存
void MenuWidget::SaveToFile(nlohmann::json* json)
{
	auto& sub = (*json);

	sub["rectTransform"] = _rectTransform;
	sub["optionsSize"] = _options.size();
	for (size_t i = 0; i < _options.size(); ++i)
	{
		std::string label = "option_" + std::to_string(i);
		auto& optionSub = sub[label];
		optionSub["label"] = _options[i].label;
		optionSub["onSelectedCallbackName"] = _options[i].onSelectedCallbackName;
		optionSub["nextWidgetName"] = _options[i].nextWidgetName;
	}

	_titleSprite.SaveToFile(&sub, "titleSprite");
	_descriptionSprite.SaveToFile(&sub, "descriptionSprite");
	_optionSprite.SaveToFile(&sub, "optionSprite");
	_selectedOptionSprite.SaveToFile(&sub, "selectedOptionSprite");

	sub["title"] = _title;
	sub["titleLabelOffset"] = _titleLabelOffset;
	sub["titleFontSize"] = _titleFontSize;
	sub["titleLabelColor"] = _titleLabelColor;
	sub["description"] = _description;
	sub["descriptionLabelOffset"] = _descriptionLabelOffset;
	sub["descriptionFontSize"] = _descriptionFontSize;
	sub["descriptionLabelColor"] = _descriptionLabelColor;

	sub["optionVerticalSpacing"] = _optionVerticalSpacing;
	sub["optionFontSize"] = _optionLabelFontSize;
	sub["optionLabelOffset"] = _optionLabelOffset;
	sub["optionLabelColor"] = _optionLabelColor;
	sub["optionLabelSelectedColor"] = _optionLabelSelectedColor;

	sub["isDrawPreviousWidget"] = _isDrawPreviousWidget;

	// ファイル保存処理
	OnSaveToFile(json);
}
#pragma endregion
// 選択肢描画処理
void MenuWidget::RenderOptions(const RenderContext& rc, MenuUIActor* owner)
{
	TextureRenderer& textureRenderer = owner->GetScene()->GetTextureRenderer();
	TextRenderer& textRenderer = owner->GetScene()->GetTextRenderer();

	int childSize = static_cast<int>(_options.size());
	RectTransform rect = _rectTransform;
	for (int i = 0; i < childSize; ++i)
	{
		auto& option = _options[i];

		// 子要素のトランスフォーム設定
		_optionSprite.UpdateTransform(&rect);
		// 背景描画
		_optionSprite.Render(rc, textureRenderer);

		// 選択中処理
		if (i == _selectedOptionIndex)
		{
			// 子要素のトランスフォーム設定
			_selectedOptionSprite.UpdateTransform(&rect);
			// 背景描画
			_selectedOptionSprite.Render(rc, textureRenderer);
		}

		// ラベル描画
		std::wstring text = ToUtf16(option.label);
		Vector2 labelPos = rect.GetWorldPosition() + _optionLabelOffset;
		textRenderer.Draw(
			FontType::MSGothic,
			text.c_str(),
			labelPos,
			i == _selectedOptionIndex ? _optionLabelSelectedColor : _optionLabelColor,
			0.0f,
			Vector2::Zero,
			_optionLabelFontSize);

		// 次の選択肢位置計算
		Vector2 position = rect.GetLocalPosition();
		position.y += _optionVerticalSpacing;
		rect.SetLocalPosition(position);
		rect.UpdateTransform(&owner->GetRectTransform());
	}
}
// タイトル描画処理
void MenuWidget::RenderTitle(const RenderContext& rc, MenuUIActor* owner)
{
	// タイトル背景
	_titleSprite.Render(rc, owner->GetScene()->GetTextureRenderer());
	if (!_title.empty())
	{
		// タイトルラベル描画
		std::wstring titleText = ToUtf16(_title);
		owner->GetScene()->GetTextRenderer().Draw(
			FontType::MSGothic,
			titleText.c_str(),
			_titleSprite.GetRectTransform().GetWorldPosition() + _titleLabelOffset,
			_titleLabelColor,
			0.0f,
			Vector2::Zero,
			_titleFontSize);
	}
}
// 説明文描画処理
void MenuWidget::RenderDescription(const RenderContext& rc, MenuUIActor* owner)
{
	// 説明文背景
	_descriptionSprite.Render(rc, owner->GetScene()->GetTextureRenderer());
	if (!_description.empty())
	{
		// 説明文ラベル描画
		std::wstring descriptionText = ToUtf16(_description);
		owner->GetScene()->GetTextRenderer().Draw(
			FontType::MSGothic,
			descriptionText.c_str(),
			_descriptionSprite.GetRectTransform().GetWorldPosition() + _descriptionLabelOffset,
			_descriptionLabelColor,
			0.0f,
			Vector2::Zero,
			_descriptionFontSize);
	}
}
// 各画像のGUI描画処理
void MenuWidget::DrawSpritesGui(MenuUIActor* owner)
{
	if (ImGui::TreeNode(u8"選択肢の背景画像"))
	{
		_optionSprite.DrawGui();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode(u8"選択肢の選択中画像"))
	{
		_selectedOptionSprite.DrawGui();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode(u8"タイトル画像"))
	{
		_titleSprite.DrawGui();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode(u8"説明文画像"))
	{
		_descriptionSprite.DrawGui();
		ImGui::TreePop();
	}
}
#pragma endregion

#pragma region 縦横に並ぶメニューUIノードエディタ用ウィジェットクラス
// 更新処理
void MenuMatrixWidget::Update(float elapsedTime, MenuUIActor* owner)
{
	MenuWidget::Update(elapsedTime, owner);
	// トランスフォーム更新
	_layerBackgroundSprite.UpdateTransform(&_rectTransform);
	_layerOptionSprite.UpdateTransform(&_rectTransform);
	_layerSelectedOptionSprite.UpdateTransform(&_rectTransform);
}
// 描画処理
void MenuMatrixWidget::Render(const RenderContext& rc, MenuUIActor* owner)
{
	// レイヤーの背景描画
	_layerBackgroundSprite.Render(rc, owner->GetScene()->GetTextureRenderer());

	// 選択肢レイヤー描画
	RenderOptionLayers(rc, owner);

	MenuWidget::Render(rc, owner);
}
// GUI描画処理
void MenuMatrixWidget::DrawGui(MenuUIActor* owner)
{
	MenuWidget::DrawGui(owner);

	if (ImGui::TreeNode(u8"選択肢レイヤー"))
	{
		static std::string newOptionLayerLabel = "NewOption";
		ImGui::InputText(u8"新規選択肢レイヤー", &newOptionLayerLabel);
		if (ImGui::Button(u8"選択肢レイヤー追加"))
		{
			AddOptionLayer(newOptionLayerLabel);
		}
		ImGui::Separator();// レイヤーごとのループ
		for (size_t layerIndex = 0; layerIndex < _optionLayers.size(); ++layerIndex)
		{
			auto& layer = _optionLayers[layerIndex];

			// IDプッシュ (レイヤー単位)
			ImGui::PushID(static_cast<int>(layerIndex));

			// ヘッダー: レイヤー名と削除ボタン
			bool headerVisible = ImGui::CollapsingHeader((std::string("Layer ") + std::to_string(layerIndex) + ": " + layer.label).c_str(), ImGuiTreeNodeFlags_DefaultOpen);

			// コンテキストメニューでレイヤー削除
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem(u8"このレイヤーを削除"))
				{
					RemoveOptionLayer(layerIndex);
					ImGui::EndPopup();
					ImGui::PopID();
					continue; // 配列がずれるのでループを抜ける
				}
				ImGui::EndPopup();
			}

			if (headerVisible)
			{
				ImGui::Indent(); // インデント開始

				// レイヤー名編集
				ImGui::InputText(u8"レイヤー名", &layer.label);

				ImGui::Spacing();
				ImGui::TextDisabled(u8"登録済みオプション (ドラッグで並び替え)");

				// --- 登録済みオプションリスト (並び替え・削除機能付き) ---
				int moveFrom = -1;
				int moveTo = -1;

				for (size_t i = 0; i < layer.optionIndices.size(); ++i)
				{
					size_t optionIdx = layer.optionIndices[i];

					// 範囲外チェック
					if (optionIdx >= _options.size()) continue;

					ImGui::PushID(static_cast<int>(i));

					// 削除ボタン
					if (ImGui::Button("X"))
					{
						layer.optionIndices.erase(layer.optionIndices.begin() + i);
						ImGui::PopID();
						i--; // インデックス調整
						continue;
					}

					ImGui::SameLine();

					// グリップアイコンのような見た目
					ImGui::Selectable("::", false, 0, ImVec2(20, 0));

					// ドラッグ＆ドロップソース
					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("LAYER_OPTION_REORDER", &i, sizeof(int));
						ImGui::Text("%s", _options[optionIdx].label.c_str());
						ImGui::EndDragDropSource();
					}
					// ドラッグ＆ドロップターゲット
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("LAYER_OPTION_REORDER"))
						{
							moveFrom = *(const int*)payload->Data;
							moveTo = static_cast<int>(i);
						}
						ImGui::EndDragDropTarget();
					}

					ImGui::SameLine();

					// オプション名
					std::string itemLabel = std::to_string(optionIdx) + ": " + _options[optionIdx].label;
					ImGui::Text("%s", itemLabel.c_str());

					ImGui::PopID();
				}

				// 並び替え処理適用
				if (moveFrom != -1 && moveTo != -1 && moveFrom != moveTo)
				{
					size_t val = layer.optionIndices[moveFrom];
					layer.optionIndices.erase(layer.optionIndices.begin() + moveFrom);
					layer.optionIndices.insert(layer.optionIndices.begin() + moveTo, val);
				}

				ImGui::Spacing();

				// --- オプション追加コンボボックス ---
				ImGui::Separator();
				if (ImGui::BeginCombo(u8"オプションを追加", u8"選択してください..."))
				{
					for (size_t k = 0; k < _options.size(); ++k)
					{
						// すでに登録済みかチェック (重複登録を防ぐ場合)
						bool isAlreadyAdded = false;
						for (auto existingIdx : layer.optionIndices) {
							if (existingIdx == k) { isAlreadyAdded = true; break; }
						}

						// 表示用ラベル
						std::string optLabel = std::to_string(k) + ": " + _options[k].label;
						if (isAlreadyAdded) {
							optLabel += " (Registered)";
						}

						if (ImGui::Selectable(optLabel.c_str(), false, isAlreadyAdded ? ImGuiSelectableFlags_Disabled : 0))
						{
							if (!isAlreadyAdded)
							{
								layer.optionIndices.push_back(k);
							}
						}
					}
					ImGui::EndCombo();
				}

				ImGui::Unindent(); // インデント終了
			}

			ImGui::PopID(); // レイヤーID Pop
			ImGui::Separator();
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode(u8"選択肢レイヤーパラメータ"))
	{
		ImGui::DragFloat2(u8"レイヤー背景画像の垂直間隔", &_layerSpacing.x);
		ImGui::DragFloat2(u8"レイヤー背景画像のフォントサイズ", &_layerLabelFontSize.x);
		ImGui::DragFloat2(u8"レイヤー選択肢画像のフォントサイズ", &_layerLabelOffset.x);
		ImGui::ColorEdit4(u8"レイヤーラベルの色", &_layerLabelColor.x);
		ImGui::ColorEdit4(u8"レイヤー選択肢ラベルの色", &_layerLabelSelectedColor.x);
		ImGui::Text(u8"選択中選択肢レイヤーインデックス: %d", static_cast<int>(_selectedOptionLayerIndex));
		ImGui::Checkbox(u8"レイヤーインデックス変更可能", &_canChangeLayerIndex);

		ImGui::TreePop();
	}
}
// 選択肢インデックス増加
int MenuMatrixWidget::AddSelectedOptionIndex()
{
	if (!_canChangeIndex)
		return _selectedOptionIndex;
	if (_optionLayers.empty())
		return _selectedOptionIndex;

	auto& layer = _optionLayers[_selectedOptionLayerIndex];
	_selectedOptionIndex++;
	// インデックス範囲制限
	_selectedOptionIndex = ClampSelectedOptionIndex(_selectedOptionIndex);
	return _selectedOptionIndex;
}
// 選択肢インデックス減少
int MenuMatrixWidget::SubSelectedOptionIndex()
{
	if (!_canChangeIndex)
		return _selectedOptionIndex;
	if (_optionLayers.empty())
		return _selectedOptionIndex;

	auto& layer = _optionLayers[_selectedOptionLayerIndex];
	_selectedOptionIndex--;
	// インデックス範囲制限
	_selectedOptionIndex = ClampSelectedOptionIndex(_selectedOptionIndex);
	return _selectedOptionIndex;
}
// インデックス範囲制限
int MenuMatrixWidget::ClampSelectedOptionIndex(int index)
{
	if (_optionLayers.empty())
		return 0;
	auto& layer = _optionLayers[_selectedOptionLayerIndex];
	if (layer.optionIndices.empty())
		return 0;
	index = index % (static_cast<int>(layer.optionIndices.size()));
	if (index < 0)
		index += static_cast<int>(layer.optionIndices.size());
	return index;
}
// 選択肢選択処理
void MenuMatrixWidget::SelectOption(MenuUIActor* owner)
{
	if (_optionLayers.empty())
		return;
	auto& layer = _optionLayers[_selectedOptionLayerIndex];
	if (layer.optionIndices.empty())
		return;
	size_t optionIndex = layer.optionIndices[_selectedOptionIndex];
	if (optionIndex >= _options.size())
		return;
	auto& option = _options[optionIndex];
	if (!option.onSelectedCallbackName.empty())
	{
		owner->CallOptionSelected(option.onSelectedCallbackName);
	}
	if (!option.nextWidgetName.empty())
	{
		owner->PushPage(option.nextWidgetName);
	}
}
// 選択肢レイヤー追加
void MenuMatrixWidget::AddOptionLayer(const std::string& label)
{
	auto& layer = _optionLayers.emplace_back();
	layer.label = label;
}
// 選択肢レイヤー削除
void MenuMatrixWidget::RemoveOptionLayer(size_t layerIndex)
{
	if (layerIndex < _optionLayers.size())
	{
		_optionLayers.erase(_optionLayers.begin() + layerIndex);
	}
}
// 選択肢レイヤーの表示する選択肢番号追加
void MenuMatrixWidget::AddOptionIndexToLayer(size_t layerIndex, size_t optionIndex)
{
	if (layerIndex >= _optionLayers.size())
		return;
	_optionLayers[layerIndex].optionIndices.push_back(optionIndex);
}
// 選択肢レイヤーの表示する選択肢番号削除
void MenuMatrixWidget::RemoveOptionIndexFromLayer(size_t layerIndex, size_t optionIndex)
{
	if (layerIndex >= _optionLayers.size())
		return;
	auto& layer = _optionLayers[layerIndex];
	layer.optionIndices.erase(
		std::remove(layer.optionIndices.begin(), layer.optionIndices.end(), optionIndex),
		layer.optionIndices.end());
}
// 選択肢レイヤーインデックス増加
int MenuMatrixWidget::AddOptionLayerIndex()
{
	if (!_canChangeLayerIndex)
		return _selectedOptionLayerIndex;

	_selectedOptionLayerIndex++;
	// インデックス範囲制限
	_selectedOptionLayerIndex = ClampOptionLayerIndex(_selectedOptionLayerIndex);
	
	// 選択肢インデックスリセット
	_selectedOptionIndex = 0;

	return _selectedOptionLayerIndex;
}
// 選択肢レイヤーインデックス減少
int MenuMatrixWidget::SubOptionLayerIndex()
{
	if (!_canChangeLayerIndex)
		return _selectedOptionLayerIndex;

	_selectedOptionLayerIndex--;
	// インデックス範囲制限
	_selectedOptionLayerIndex = ClampOptionLayerIndex(_selectedOptionLayerIndex);

	// 選択肢インデックスリセット
	_selectedOptionIndex = 0;

	return _selectedOptionLayerIndex;
}
// 選択肢レイヤーインデックス範囲制限
int MenuMatrixWidget::ClampOptionLayerIndex(int index)
{
	if (_optionLayers.empty())
		return 0;
	index = index % (static_cast<int>(_optionLayers.size()));
	if (index < 0)
		index += static_cast<int>(_optionLayers.size());
	return index;
}
// ファイル読み込み処理
void MenuMatrixWidget::OnLoadFromFile(nlohmann::json* json)
{
	_optionLayers.clear();
	size_t optionLayerSize = (*json).value("optionLayerSize", 0);
	for (size_t i = 0; i < optionLayerSize; ++i)
	{
		std::string label = "optionLayer_" + std::to_string(i);
		auto& layerSub = (*json)[label];
		OptionLayer layer;
		layer.label = layerSub.value("label", "");
		size_t optionIndicesSize = layerSub.value("optionIndicesSize", 0);
		for (size_t j = 0; j < optionIndicesSize; ++j)
		{
			layer.optionIndices.push_back(layerSub.value("optionIndex_" + std::to_string(j), 0));
		}
		_optionLayers.push_back(layer);
	}
	_layerBackgroundSprite.LoadFromFile(json, "layerBackgroundSprite");
	_layerOptionSprite.LoadFromFile(json, "layerOptionSprite");
	_layerSelectedOptionSprite.LoadFromFile(json, "layerSelectedOptionSprite");
	_layerSpacing = (*json).value("layerSpacing", Vector2(0.0f, 100.0f));
	_layerLabelFontSize = (*json).value("layerLabelFontSize", Vector2(1.0f, 1.0f));
	_layerLabelOffset = (*json).value("layerLabelOffset", Vector2(20.0f, 10.0f));
	_layerLabelColor = (*json).value("layerLabelColor", Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	_layerLabelSelectedColor = (*json).value("layerLabelSelectedColor", Vector4(1.0f, 0.0f, 0.0f, 1.0f));
}
// ファイル保存処理
void MenuMatrixWidget::OnSaveToFile(nlohmann::json* json)
{
	(*json)["optionLayerSize"] = _optionLayers.size();
	for (size_t i = 0; i < _optionLayers.size(); ++i)
	{
		std::string label = "optionLayer_" + std::to_string(i);
		auto& layerSub = (*json)[label];
		auto& layer = _optionLayers[i];
		layerSub["label"] = layer.label;
		layerSub["optionIndicesSize"] = layer.optionIndices.size();
		for (size_t j = 0; j < layer.optionIndices.size(); ++j)
		{
			layerSub["optionIndex_" + std::to_string(j)] = layer.optionIndices[j];
		}
	}

	_layerBackgroundSprite.SaveToFile(json, "layerBackgroundSprite");
	_layerOptionSprite.SaveToFile(json, "layerOptionSprite");
	_layerSelectedOptionSprite.SaveToFile(json, "layerSelectedOptionSprite");

	(*json)["layerSpacing"]				= _layerSpacing;
	(*json)["layerLabelFontSize"]		= _layerLabelFontSize;
	(*json)["layerLabelOffset"]			= _layerLabelOffset;
	(*json)["layerLabelColor"]			= _layerLabelColor;
	(*json)["layerLabelSelectedColor"]	= _layerLabelSelectedColor;
}
// 選択肢描画処理
void MenuMatrixWidget::RenderOptions(const RenderContext& rc, MenuUIActor* owner)
{
	// 選択中のレイヤーに含まれる選択肢を描画
	if (_optionLayers.empty())
		return;
	auto& layer = _optionLayers[_selectedOptionLayerIndex];

	TextureRenderer& textureRenderer = owner->GetScene()->GetTextureRenderer();
	TextRenderer& textRenderer = owner->GetScene()->GetTextRenderer();

	int i = 0;
	RectTransform rect = _rectTransform;
	for (auto index : layer.optionIndices)
	{
		// 範囲チェック
		if (index >= _options.size())
			continue;

		auto& option = _options[index];
		// 子要素のトランスフォーム設定
		_optionSprite.UpdateTransform(&rect);
		// 背景描画
		_optionSprite.Render(rc, textureRenderer);

		// 選択中処理
		if (i == _selectedOptionIndex)
		{
			// 子要素のトランスフォーム設定
			_selectedOptionSprite.UpdateTransform(&rect);
			// 背景描画
			_selectedOptionSprite.Render(rc, textureRenderer);
		}

		// ラベル描画
		std::wstring text = ToUtf16(option.label);
		Vector2 labelPos = rect.GetWorldPosition() + _optionLabelOffset;
		textRenderer.Draw(
			FontType::MSGothic,
			text.c_str(),
			labelPos,
			i == _selectedOptionIndex ? _optionLabelSelectedColor : _optionLabelColor,
			0.0f,
			Vector2::Zero,
			_optionLabelFontSize);

		// 次の選択肢位置計算
		Vector2 position = rect.GetLocalPosition();
		position.y += _optionVerticalSpacing;
		rect.SetLocalPosition(position);
		rect.UpdateTransform(&owner->GetRectTransform());

		i++;
	}
}
// 選択肢レイヤー描画処理
void MenuMatrixWidget::RenderOptionLayers(const RenderContext& rc, MenuUIActor* owner)
{
	TextureRenderer& textureRenderer = owner->GetScene()->GetTextureRenderer();
	TextRenderer& textRenderer = owner->GetScene()->GetTextRenderer();

	int childSize = static_cast<int>(_optionLayers.size());
	RectTransform rect = _rectTransform;
	for (int i = 0; i < childSize; ++i)
	{
		auto& layer = _optionLayers[i];

		// 子要素のトランスフォーム設定
		_layerOptionSprite.UpdateTransform(&rect);
		// 背景描画
		_layerOptionSprite.Render(rc, textureRenderer);

		// 選択中処理
		if (i == _selectedOptionLayerIndex)
		{
			// 子要素のトランスフォーム設定
			_layerSelectedOptionSprite.UpdateTransform(&rect);
			// 背景描画
			_layerSelectedOptionSprite.Render(rc, textureRenderer);
		}

		// ラベル描画
		std::wstring text = ToUtf16(layer.label);
		Vector2 labelPos = rect.GetWorldPosition() + _layerLabelOffset;
		textRenderer.Draw(
			FontType::MSGothic,
			text.c_str(),
			labelPos,
			i == _selectedOptionLayerIndex ? _layerLabelSelectedColor : _layerLabelColor,
			0.0f,
			Vector2::Zero,
			_layerLabelFontSize);

		// 次の選択肢位置計算
		Vector2 position = rect.GetLocalPosition();
		position += _layerSpacing;
		rect.SetLocalPosition(position);
		rect.UpdateTransform(&owner->GetRectTransform());
	}
}
// 各画像のGUI描画処理
void MenuMatrixWidget::DrawSpritesGui(MenuUIActor* owner)
{
	MenuWidget::DrawSpritesGui(owner);
	if (ImGui::TreeNode(u8"レイヤーの背景画像"))
	{
		_layerBackgroundSprite.DrawGui();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode(u8"レイヤーの選択肢画像"))
	{
		_layerOptionSprite.DrawGui();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode(u8"レイヤーの選択中画像"))
	{
		_layerSelectedOptionSprite.DrawGui();
		ImGui::TreePop();
	}
}
#pragma endregion

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
