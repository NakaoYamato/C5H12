#include "MenuUIActor.h"

#include "../../Library/Scene/Scene.h"

#include <Mygui.h>

// 生成時処理
void MenuUIActor::OnCreate()
{
	UIActor::OnCreate();

	std::unique_ptr<MenuWidget> mainMenu = std::make_unique<MenuWidget>("MainMenu");
	mainMenu->AddOption("Pop001", [](MenuUIActor* owner)
		{
			std::unique_ptr<MenuWidget> pop001 = std::make_unique<MenuWidget>("pop001");
			pop001->AddOption("pop001-001", [](MenuUIActor* owner)
				{
					owner->PopPage();
				});
			pop001->AddOption("pop001-002", [](MenuUIActor* owner)
				{
					owner->PopPage();
				});
			owner->PushPage(std::move(pop001));
		});
	mainMenu->AddOption("Pop002", [](MenuUIActor* owner)
		{
			std::unique_ptr<MenuWidget> pop002 = std::make_unique<MenuWidget>("pop001");
			pop002->AddOption("pop002-001", [](MenuUIActor* owner)
				{
					owner->PopPage();
				});
			pop002->AddOption("pop002-002", [](MenuUIActor* owner)
				{
					owner->PopPage();
				});
			owner->PushPage(std::move(pop002));
		});
	mainMenu->AddOption("Pop003", [](MenuUIActor* owner)
		{
			std::unique_ptr<MenuWidget> pop003 = std::make_unique<MenuWidget>("pop001");
			pop003->AddOption("pop003-001", [](MenuUIActor* owner)
				{
					owner->PopPage();
				});
			pop003->AddOption("pop003-002", [](MenuUIActor* owner)
				{
					owner->PopPage();
				});
			owner->PushPage(std::move(pop003));
		});
	mainMenu->AddOption("Pop004", [](MenuUIActor* owner)
		{
			std::unique_ptr<MenuWidget> pop004 = std::make_unique<MenuWidget>("pop001");
			pop004->AddOption("pop004-001", [](MenuUIActor* owner)
				{
					owner->PopPage();
				});
			pop004->AddOption("pop004-002", [](MenuUIActor* owner)
				{
					owner->PopPage();
				});
			owner->PushPage(std::move(pop004));
		});
	PushPage(std::move(mainMenu));
}

// 更新時処理
void MenuUIActor::OnUpdate(float elapsedTime)
{
	UIActor::OnUpdate(elapsedTime);
	if (!_widgetStack.empty())
	{
		_widgetStack.top()->Update(elapsedTime, this);
	}
}

// 3D描画後の描画時処理
void MenuUIActor::OnDelayedRender(const RenderContext& rc)
{
	UIActor::OnDelayedRender(rc);
	if (!_widgetStack.empty())
	{
		_widgetStack.top()->Render(rc, this);
	}
}

// トランスフォーム更新
void MenuUIActor::UpdateTransform()
{
	UIActor::UpdateTransform();
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
			if (!_widgetStack.empty())
			{
				_widgetStack.top()->DrawGui(this);
			}

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

void MenuUIActor::PushPage(std::unique_ptr<MenuWidget> page)
{
	if (page)
	{
		// 現在のページがあれば、終了時の処理
		if (!_widgetStack.empty())
		{
			_widgetStack.top()->OnExit();
		}

		page->OnEnter();
		_widgetStack.push(std::move(page));
	}
}

void MenuUIActor::PopPage()
{
	if (!_widgetStack.empty())
	{
		_widgetStack.top()->OnExit();
		_widgetStack.pop();

		// 戻った先のページがあれば、再開時の処理
		if (!_widgetStack.empty())
		{
			_widgetStack.top()->OnEnter();
		}
	}
}

#pragma region MenuWidget
// 更新処理
void MenuWidget::Update(float elapsedTime, MenuUIActor* owner)
{
	// 入力処理
	if (_INPUT_REPEAT("Up"))
		_selectedOptionIndex--;
	if (_INPUT_REPEAT("Down"))
		_selectedOptionIndex++;
	// インデックス範囲制限
	_selectedOptionIndex = _selectedOptionIndex % (static_cast<int>(_options.size()));
	if (_selectedOptionIndex < 0)
		_selectedOptionIndex += static_cast<int>(_options.size());

	if (_INPUT_TRIGGERD("Select"))
	{
		auto& option = _options[_selectedOptionIndex];
		if (option.onSelect)
		{
			option.onSelect(owner);
		}
	}
	if (_INPUT_TRIGGERD("Back") || _INPUT_TRIGGERD("Menu"))
	{
		owner->PopPage();
	}

	// トランスフォーム更新
	_rectTransform.UpdateTransform(&owner->GetRectTransform());
}
// 描画処理
void MenuWidget::Render(const RenderContext& rc, MenuUIActor* owner)
{
	int childSize = static_cast<int>(_options.size());
	RectTransform rect = _rectTransform;
	for (int i = 0; i < childSize; ++i)
	{
		auto& option = _options[i];

		Sprite* spr = i == _selectedOptionIndex ?
			&_selectedOptionSprite :
			&_optionSprite;
		// 子要素のトランスフォーム設定
		spr->UpdateTransform(&rect);
		
		// 背景描画
		spr->Render(rc, owner->GetScene()->GetTextureRenderer());

		// ラベル描画
		Vector2 labelPos = rect.GetWorldPosition() + _optionLabelOffset;
		TextRenderer& textRenderer = owner->GetScene()->GetTextRenderer();
		textRenderer.Draw(
			FontType::MSGothic,
			option.label.c_str(),
			labelPos,
			i == _selectedOptionIndex ? _optionLabelSelectedColor : _optionLabelColor,
			0.0f,
			Vector2::Zero,
			_optionFontSize);

		// 次のオプション位置計算
		Vector2 position = rect.GetLocalPosition();
		position.y += _optionVerticalSpacing;
		rect.SetLocalPosition(position);
		rect.UpdateTransform(&owner->GetRectTransform());
	}
}
// GUI描画処理
void MenuWidget::DrawGui(MenuUIActor* owner)
{
	ImGui::Text(u8"ウィジェット名: %s", _name.c_str());
	_rectTransform.DrawGui();

	if (ImGui::TreeNode(u8"オプションリスト"))
	{
		for (size_t i = 0; i < _options.size(); ++i)
		{
			ImGui::PushID(static_cast<int>(i));

			auto& option = _options[i];
			ImGui::InputText(("オプション " + std::to_string(i) + " ラベル").c_str(), &option.label);

			ImGui::PopID();
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode(u8"オプションの背景画像"))
	{
		_optionSprite.DrawGui();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode(u8"選択中オプション画像"))
	{
		_selectedOptionSprite.DrawGui();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode(u8"パラメータ"))
	{
		ImGui::DragFloat(u8"オプション垂直間隔", &_optionVerticalSpacing);
		ImGui::DragFloat2(u8"ラベルフォントサイズ", &_optionFontSize.x);
		ImGui::DragFloat2(u8"ラベルオフセット", &_optionLabelOffset.x);
		ImGui::ColorEdit4(u8"ラベル色", &_optionLabelColor.x);
		ImGui::ColorEdit4(u8"選択中ラベル色", &_optionLabelSelectedColor.x);

		ImGui::Text(u8"選択中オプションインデックス: %d", static_cast<int>(_selectedOptionIndex));
		
		ImGui::TreePop();
	}
}
#pragma endregion
