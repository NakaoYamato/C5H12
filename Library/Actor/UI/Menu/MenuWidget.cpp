#include "MenuWidget.h"

#include "MenuUIActor.h"
#include "../../Library/Scene/Scene.h"

#include <Mygui.h>

// コピー用ポインタ
MenuWidget* CopyMenuWidgetPtr = nullptr;

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
			this->_optionDisabledSprite = CopyMenuWidgetPtr->_optionDisabledSprite;
			this->_optionVerticalSpacing = CopyMenuWidgetPtr->_optionVerticalSpacing;
			this->_optionLabelFontSize = CopyMenuWidgetPtr->_optionLabelFontSize;
			this->_optionLabelOffset = CopyMenuWidgetPtr->_optionLabelOffset;
			this->_optionLabelColor = CopyMenuWidgetPtr->_optionLabelColor;
			this->_optionLabelSelectedColor = CopyMenuWidgetPtr->_optionLabelSelectedColor;
			this->_optionLabelDisabledColor = CopyMenuWidgetPtr->_optionLabelDisabledColor;
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
			if (ImGui::TreeNode(u8"選択時遷移先"))
			{
				ImGui::BeginChild("NextWidgetNameList", ImVec2(0, 150), true);
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
				ImGui::EndChild();
				ImGui::TreePop();
			}
			if (ImGui::TreeNode(u8"選択可能かのコールバック"))
			{
				ImGui::BeginChild("CanSelectCallBackList", ImVec2(0, 150), true);
				if (ImGui::RadioButton(u8"なし", option.canSelectCallbackName.empty()))
				{
					option.canSelectCallbackName.clear();
				}
				for (auto& str : owner->GetCanSelectOptionCallbackHandler().GetCallBackNames())
				{
					if (ImGui::RadioButton(str.c_str(), option.canSelectCallbackName == str))
					{
						option.canSelectCallbackName = str;
					}
				}
				ImGui::EndChild();
				ImGui::TreePop();
			}
			if (ImGui::TreeNode(u8"選択時コールバック"))
			{
				ImGui::BeginChild("SelectedCallBackList", ImVec2(0, 150), true);
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
				ImGui::EndChild();
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
		ImGui::ColorEdit4(u8"選択不可ラベル色", &_optionLabelDisabledColor.x);

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
void MenuWidget::AddOption(const std::string& label,
	const std::string& nextWidgetName,
	const std::string& canSelectCallbackName,
	const std::string& onSelectedCallbackName)
{
	auto& option = _options.emplace_back();
	option.label = label;
	option.nextWidgetName = nextWidgetName;
	option.canSelectCallbackName = canSelectCallbackName;
	option.onSelectedCallbackName = onSelectedCallbackName;
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
	// 選択可能か確認
	if (!option.canSelectCallbackName.empty())
	{
		bool canSelect = owner->CallCanSelectOption(option.canSelectCallbackName);
		if (!canSelect)
			return;
	}
	// 選択時コールバック呼び出し
	if (!option.onSelectedCallbackName.empty())
	{
		owner->CallOptionSelected(option.onSelectedCallbackName);
	}
	// 遷移先ウィジェットがあれば遷移
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
		option.nextWidgetName = optionSub.value("nextWidgetName", "");
		option.canSelectCallbackName = optionSub.value("canSelectCallbackName", "");
		option.onSelectedCallbackName = optionSub.value("onSelectedCallbackName", "");
		_options.push_back(option);
	}

	_titleSprite.LoadFromFile(&sub, "titleSprite");
	_descriptionSprite.LoadFromFile(&sub, "descriptionSprite");
	_optionSprite.LoadFromFile(&sub, "optionSprite");
	_selectedOptionSprite.LoadFromFile(&sub, "selectedOptionSprite");
	_optionDisabledSprite.LoadFromFile(&sub, "optionDisabledSprite");

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
	_optionLabelDisabledColor = sub.value("optionLabelDisabledColor", Vector4(0.2f, 0.2f, 0.2f, 1.0f));

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
		optionSub["nextWidgetName"] = _options[i].nextWidgetName;
		optionSub["canSelectCallbackName"] = _options[i].canSelectCallbackName;
		optionSub["onSelectedCallbackName"] = _options[i].onSelectedCallbackName;
	}

	_titleSprite.SaveToFile(&sub, "titleSprite");
	_descriptionSprite.SaveToFile(&sub, "descriptionSprite");
	_optionSprite.SaveToFile(&sub, "optionSprite");
	_selectedOptionSprite.SaveToFile(&sub, "selectedOptionSprite");
	_optionDisabledSprite.SaveToFile(&sub, "optionDisabledSprite");

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
	sub["optionLabelDisabledColor"] = _optionLabelDisabledColor;

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

		// 状態を確認
		// -1: 選択不可、0: 通常、1: 選択中
		int state = i == _selectedOptionIndex ? 1 : 0;
		if (!option.canSelectCallbackName.empty())
		{
			bool canSelect = owner->CallCanSelectOption(option.canSelectCallbackName);
			if (!canSelect)
				state = -1;
		}

		Vector4 color = _optionLabelColor;
		switch (state)
		{
		case -1:
			// 子要素のトランスフォーム設定
			_optionDisabledSprite.UpdateTransform(&rect);
			// 背景描画
			_optionDisabledSprite.Render(rc, textureRenderer);

			color = _optionLabelDisabledColor;
			break;
		case 0:
			// 子要素のトランスフォーム設定
			_optionSprite.UpdateTransform(&rect);
			// 背景描画
			_optionSprite.Render(rc, textureRenderer);

			color = _optionLabelColor;
			break;
		case 1:
			// 子要素のトランスフォーム設定
			_selectedOptionSprite.UpdateTransform(&rect);
			// 背景描画
			_selectedOptionSprite.Render(rc, textureRenderer);

			color = _optionLabelSelectedColor;
			break;
		}

		// ラベル描画
		std::wstring text = ToUtf16(option.label);
		Vector2 labelPos = rect.GetWorldPosition() + _optionLabelOffset;
		textRenderer.Draw(
			FontType::MSGothic,
			text.c_str(),
			labelPos,
			color,
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

	if (ImGui::TreeNode(u8"選択肢の選択不可画像"))
	{
		_optionDisabledSprite.DrawGui();
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

	(*json)["layerSpacing"] = _layerSpacing;
	(*json)["layerLabelFontSize"] = _layerLabelFontSize;
	(*json)["layerLabelOffset"] = _layerLabelOffset;
	(*json)["layerLabelColor"] = _layerLabelColor;
	(*json)["layerLabelSelectedColor"] = _layerLabelSelectedColor;
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

		// 状態を確認
		// -1: 選択不可、0: 通常、1: 選択中
		int state = i == _selectedOptionIndex ? 1 : 0;
		if (!option.canSelectCallbackName.empty())
		{
			bool canSelect = owner->CallCanSelectOption(option.canSelectCallbackName);
			if (!canSelect)
				state = -1;
		}

		Vector4 color = _optionLabelColor;
		switch (state)
		{
		case -1:
			// 子要素のトランスフォーム設定
			_optionDisabledSprite.UpdateTransform(&rect);
			// 背景描画
			_optionDisabledSprite.Render(rc, textureRenderer);

			color = _optionLabelDisabledColor;
			break;
		case 0:
			// 子要素のトランスフォーム設定
			_optionSprite.UpdateTransform(&rect);
			// 背景描画
			_optionSprite.Render(rc, textureRenderer);

			color = _optionLabelColor;
			break;
		case 1:
			// 子要素のトランスフォーム設定
			_selectedOptionSprite.UpdateTransform(&rect);
			// 背景描画
			_selectedOptionSprite.Render(rc, textureRenderer);

			color = _optionLabelSelectedColor;
			break;
		}

		// ラベル描画
		std::wstring text = ToUtf16(option.label);
		Vector2 labelPos = rect.GetWorldPosition() + _optionLabelOffset;
		textRenderer.Draw(
			FontType::MSGothic,
			text.c_str(),
			labelPos,
			color,
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

#pragma region チェックボックスウィジェット
// 選択肢選択処理
void MenuCheckBoxWidget::SelectOption(MenuUIActor* owner)
{
	if (_options.empty())
		return;

	auto& option = _options[_selectedOptionIndex];
	// 選択時コールバック呼び出し
	if (!option.onSelectedCallbackName.empty())
	{
		owner->CallOptionSelected(option.onSelectedCallbackName);
	}
	// 遷移先ウィジェットがあれば遷移
	if (!option.nextWidgetName.empty())
	{
		owner->PushPage(option.nextWidgetName);
	}
}

#pragma region ファイル
// ファイル読み込み処理
void MenuCheckBoxWidget::OnLoadFromFile(nlohmann::json* json)
{
	_checkedOptionSprite.LoadFromFile(json, "checkedOptionSprite");
	_uncheckedOptionSprite.LoadFromFile(json, "uncheckedOptionSprite");
}

// ファイル保存処理
void MenuCheckBoxWidget::OnSaveToFile(nlohmann::json* json)
{
	_checkedOptionSprite.SaveToFile(json, "checkedOptionSprite");
	_uncheckedOptionSprite.SaveToFile(json, "uncheckedOptionSprite");
}
#pragma endregion

// 選択肢描画処理
void MenuCheckBoxWidget::RenderOptions(const RenderContext& rc, MenuUIActor* owner)
{
	TextureRenderer& textureRenderer = owner->GetScene()->GetTextureRenderer();
	TextRenderer& textRenderer = owner->GetScene()->GetTextRenderer();

	int childSize = static_cast<int>(_options.size());
	RectTransform rect = _rectTransform;
	for (int i = 0; i < childSize; ++i)
	{
		auto& option = _options[i];

		bool isSelected = (i == _selectedOptionIndex);
		if (isSelected)
		{
			// 選択中枠描画
			_selectedOptionSprite.UpdateTransform(&rect);
			_selectedOptionSprite.Render(rc, textureRenderer);
		}
		else
		{
			// 通常枠描画
			_optionSprite.UpdateTransform(&rect);
			_optionSprite.Render(rc, textureRenderer);
		}

		if (!option.canSelectCallbackName.empty())
		{
			if (owner->CallCanSelectOption(option.canSelectCallbackName))
			{
				// チェック済み画像描画
				_checkedOptionSprite.UpdateTransform(&rect);
				_checkedOptionSprite.Render(rc, textureRenderer);
			}
			else
			{
				// チェック無し画像描画
				_uncheckedOptionSprite.UpdateTransform(&rect);
				_uncheckedOptionSprite.Render(rc, textureRenderer);
			}
		}

		// ラベル描画
		std::wstring text = ToUtf16(option.label);
		Vector4 color = _optionLabelColor;
		Vector2 labelPos = rect.GetWorldPosition() + _optionLabelOffset;
		textRenderer.Draw(
			FontType::MSGothic,
			text.c_str(),
			labelPos,
			isSelected ? _optionLabelSelectedColor : _optionLabelColor,
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
// 各画像のGUI描画処理
void MenuCheckBoxWidget::DrawSpritesGui(MenuUIActor* owner)
{
	MenuWidget::DrawSpritesGui(owner);
	if (ImGui::TreeNode(u8"チェック済み画像"))
	{
		_checkedOptionSprite.DrawGui();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode(u8"チェック無し画像"))
	{
		_uncheckedOptionSprite.DrawGui();
		ImGui::TreePop();
	}
}
#pragma endregion
