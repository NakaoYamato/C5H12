#include "MenuUIActor.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Algorithm/Converter.h"

#include <Mygui.h>

#define DEBUG_MENUUIACTOR 0 // 使用例

// 生成時処理
void MenuUIActor::OnCreate()
{
	UIActor::OnCreate();

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
}

// 更新時処理
void MenuUIActor::OnUpdate(float elapsedTime)
{
	UIActor::OnUpdate(elapsedTime);
	if (auto currentWidget = GetCurrentWidget())
	{
#if DEBUG_MENUUIACTOR // 使用例
		// 入力処理
		if (_INPUT_REPEAT("Up"))
			currentWidget->SubSelectedOptionIndex();
		if (_INPUT_REPEAT("Down"))
			currentWidget->AddSelectedOptionIndex();

		if (_INPUT_TRIGGERD("Select"))
		{
			currentWidget->SelectOption(this);
		}
		if (_INPUT_TRIGGERD("Back") || _INPUT_TRIGGERD("Menu"))
		{
			currentWidget->BackOption(this);
		}
#endif
		currentWidget->Update(elapsedTime, this);
	}
}

// 3D描画後の描画時処理
void MenuUIActor::OnDelayedRender(const RenderContext& rc)
{
	UIActor::OnDelayedRender(rc);
	if (auto currentWidget = GetCurrentWidget())
	{
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
			DrawWidgetGui();

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
}

// ウィジェット登録
std::shared_ptr<MenuWidget> MenuUIActor::RegisterWidget(std::shared_ptr<MenuWidget> widget)
{
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

		auto& widget = _registeredWidgets.at(name);
		if (widget) widget->OnEnter();
		_widgetStackNames.push(name);
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
		if (name.empty())
			continue;
		auto& widget = _registeredWidgets[name];
		if (widget)
		{
			widget->LoadFromFile(&sub);
		}
		else
		{
			widget = RegisterWidget(std::make_shared<MenuWidget>(name));
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
		widget->SaveToFile(&sub);
		index++;
	}

	return Exporter::SaveJsonFile(_filepath.c_str(), json);
}
#pragma endregion

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
			_optionFontSize);

		// 次の選択肢位置計算
		Vector2 position = rect.GetLocalPosition();
		position.y += _optionVerticalSpacing;
		rect.SetLocalPosition(position);
		rect.UpdateTransform(&owner->GetRectTransform());
	}

	// タイトル描画
	RenderTitle(rc, owner);

	// 説明文描画
	RenderDescription(rc, owner);
}
// GUI描画処理
void MenuWidget::DrawGui(MenuUIActor* owner)
{
	if (ImGui::TreeNode(u8"トランスフォーム"))
	{
		_rectTransform.DrawGui();
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

			ImGui::PopID();
		}

		ImGui::TreePop();
	}

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

	if (ImGui::TreeNode(u8"パラメータ"))
	{
		ImGui::DragFloat(u8"選択肢垂直間隔", &_optionVerticalSpacing);
		ImGui::DragFloat2(u8"ラベルフォントサイズ", &_optionFontSize.x);
		ImGui::DragFloat2(u8"ラベルオフセット", &_optionLabelOffset.x);
		ImGui::ColorEdit4(u8"ラベル色", &_optionLabelColor.x);
		ImGui::ColorEdit4(u8"選択中ラベル色", &_optionLabelSelectedColor.x);

		ImGui::Text(u8"選択中選択肢インデックス: %d", static_cast<int>(_selectedOptionIndex));

		ImGui::Checkbox(u8"インデックス変更可能", &_canChangeIndex);
		
		ImGui::TreePop();
	}

	if (ImGui::TreeNode(u8"タイトル"))
	{
		ImGui::InputText(u8"タイトルテキスト", &_title);
		_titleSprite.DrawGui();
		ImGui::DragFloat2(u8"タイトルオフセット", &_titleLabelOffset.x);
		ImGui::DragFloat2(u8"タイトルフォントサイズ", &_titleFontSize.x);
		ImGui::ColorEdit4(u8"タイトル色", &_titleLabelColor.x);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode(u8"説明文"))
	{
		ImGui::InputText(u8"説明文テキスト", &_description);
		_descriptionSprite.DrawGui();
		ImGui::DragFloat2(u8"説明文オフセット", &_descriptionLabelOffset.x);
		ImGui::DragFloat2(u8"説明文フォントサイズ", &_descriptionFontSize.x);
		ImGui::ColorEdit4(u8"説明文色", &_descriptionLabelColor.x);

		ImGui::TreePop();
	}
}

#pragma region 選択肢
void MenuWidget::AddOption(const std::string& label, const std::string& onSelectedCallbackName, const std::string& nextWidgetName)
{
	auto& option = _options.emplace_back();
	option.label = label;
	option.onSelectedCallbackName = onSelectedCallbackName;
	option.nextWidgetName = nextWidgetName;
}
// 選択肢インデックス増加
size_t MenuWidget::AddSelectedOptionIndex()
{
	if (!_canChangeIndex)
		return _selectedOptionIndex;

	_selectedOptionIndex++;
	// インデックス範囲制限
	_selectedOptionIndex = ClampSelectedOptionIndex(_selectedOptionIndex);
	return _selectedOptionIndex;
}
// 選択肢インデックス減少
size_t MenuWidget::SubSelectedOptionIndex()
{
	if (!_canChangeIndex)
		return _selectedOptionIndex;

	_selectedOptionIndex--;
	// インデックス範囲制限
	_selectedOptionIndex = ClampSelectedOptionIndex(_selectedOptionIndex);
	return _selectedOptionIndex;
}
// インデックス範囲制限
size_t MenuWidget::ClampSelectedOptionIndex(size_t index)
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
	_optionFontSize = sub.value("optionFontSize", Vector2(1.0f, 1.0f));
	_optionLabelOffset = sub.value("optionLabelOffset", Vector2(20.0f, 10.0f));
	_optionLabelColor = sub.value("optionLabelColor", Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	_optionLabelSelectedColor = sub.value("optionLabelSelectedColor", Vector4(1.0f, 0.0f, 0.0f, 1.0f));

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
	sub["optionFontSize"] = _optionFontSize;
	sub["optionLabelOffset"] = _optionLabelOffset;
	sub["optionLabelColor"] = _optionLabelColor;
	sub["optionLabelSelectedColor"] = _optionLabelSelectedColor;

	// ファイル保存処理
	OnSaveToFile(json);
}
#pragma endregion
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
#pragma endregion
