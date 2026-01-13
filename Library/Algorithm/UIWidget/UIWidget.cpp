#include "UIWidget.h"

#include <Mygui.h>

// 更新
void UIWidget::Update(float elapsedTime)
{
	// 起動チェック
	if (!_isActive) return;

	OnUpdate(elapsedTime);
	for (auto& child : _children) child->Update(elapsedTime);
}
// 描画
void UIWidget::Render(Scene* scene)
{
	// 起動チェック
	if (!_isActive) return;

	OnRender(scene);
    for (auto& child : _children) child->Render(scene);
}
// トランスフォーム更新
void UIWidget::UpdateTransform(const RectTransform* parent)
{
	_rectTransform.UpdateTransform(parent);
	// 子要素にも伝播
	for (auto& child : _children)
	{
		child->UpdateTransform(&_rectTransform);
	}
}

#pragma region 共通インターフェース
// 起動時
void UIWidget::OnActivated()
{
	// 子要素も起動
	for (auto& child : _children)
	{
		child->SetActive(true);
	}
}
// 停止時
void UIWidget::OnDeactivated()
{
	// 子要素も停止
	for (auto& child : _children)
	{
		child->SetActive(false);
	}
}
// フォーカスされた時
void UIWidget::OnFocused()
{
	// 子要素もフォーカス
	for (auto& child : _children)
	{
		child->SetFocus(true);
	}
}
// フォーカスが外れた時
void UIWidget::OnUnfocused()
{
	// 子要素もフォーカス解除
	for (auto& child : _children)
	{
		child->SetFocus(false);
	}
}
#pragma endregion

#pragma region アクセサ
void UIWidget::SetActive(bool active)
{
	_isActive = active;
	if (active)
	{
		OnActivated();
	}
	else
	{
		OnDeactivated();
	}
}
void UIWidget::SetFocus(bool focus)
{
	_isFocused = focus;
	if (focus)
	{
		OnFocused();
	}
	else
	{
		OnUnfocused();
	}
}
#pragma endregion

#pragma region デバッグ
// GUI描画
// GUIで選択した子供のウィジェットを返す場合がある
std::shared_ptr<UIWidget> UIWidget::DrawGui()
{
	ImGui::Text(u8"ウィジェット名:");
	ImGui::SameLine();
	ImGui::Text(_name.c_str());
	ImGui::Checkbox(u8"起動", &_isActive);
	ImGui::Checkbox(u8"フォーカス", &_isFocused);
	ImGui::Separator();
	_rectTransform.DrawGui();
	ImGui::Separator();

	OnDrawGui();
	ImGui::Separator();

	std::shared_ptr<UIWidget> selectedChild = nullptr;
	if (ImGui::TreeNode(u8"子供"))
	{
		for (auto& child : _children)
		{
			ImGui::PushID(child.get());
			ImGui::Text(child->GetName().c_str());
			ImGui::SameLine();
			if (ImGui::Button(u8">"))
			{
				selectedChild = child;
			}
			ImGui::PopID();
		}

		ImGui::TreePop();
	}
	// 親ウィジェットに戻るボタン
	if (_parent.lock())
	{
		if (ImGui::Button(u8"< 親ウィジェットに戻る"))
		{
			selectedChild = _parent.lock();
		}
	}

	return selectedChild;
}
#pragma endregion

#pragma region UIImage
// 描画
void UIImage::OnRender(Scene* scene)
{
	auto& rc = scene->GetRenderContext();
	auto& textureRenderer = scene->GetTextureRenderer();
	_sprite.Render(rc, textureRenderer);
}
// GUI描画
void UIImage::OnDrawGui()
{
	_sprite.DrawGui();
}

// 画像読み込み
void UIImage::LoadTexture(const wchar_t* filename, Sprite::CenterAlignment alignment)
{
	_sprite.LoadTexture(filename, alignment);
}
#pragma endregion

#pragma region UIText
// 更新
void UIText::OnUpdate(float elapsedTime)
{
	_textData.position = GetRectTransform().GetWorldPosition();
	_textData.rotation = GetRectTransform().GetWorldAngle();

	// フォーカスされている場合、色を変更
	if (IsFocused())
		_textData.color = _focusColor;
	else
		_textData.color = _normalColor;
}
// 描画
void UIText::OnRender(Scene* scene)
{
	auto& textRenderer = scene->GetTextRenderer();
	textRenderer.Draw(_textData);
}
// GUI描画
void UIText::OnDrawGui()
{
	_textData.DrawGui(u8"テキスト");
	ImGui::ColorEdit4(u8"通常色", &_normalColor.x);
	ImGui::ColorEdit4(u8"フォーカス色", &_focusColor.x);
}
#pragma endregion

#pragma region UIVerticalBox
// 更新
void UIVerticalBox::OnUpdate(float elapsedTime)
{
	// 入力処理
	if (_INPUT_REPEAT("Up"))
		_selectedIndex--;
	if (_INPUT_REPEAT("Down"))
		_selectedIndex++;
	// インデックス範囲制限
	_selectedIndex = _selectedIndex % (static_cast<int>(_children.size()));
	if (_selectedIndex < 0)
		_selectedIndex += static_cast<int>(_children.size());

	int childSize = static_cast<int>(_children.size());
	float currentY = 0;
	for (int i = 0; i < childSize; ++i)
	{
		auto& child = _children[i];
		// 起動チェック
		if (!child->IsActive()) return;

		// 縦方向に子要素を配置
		child->GetRectTransform().SetLocalPosition(Vector2(0.0f, currentY));
		child->GetRectTransform().UpdateTransform(&_rectTransform);
		currentY += _height + _padding; // 高さ + 余白

		// 選択中の子要素にフォーカスを設定
		if (i == _selectedIndex)
			child->SetFocus(true);
		else
			child->SetFocus(false);
	}
}

// GUI描画
void UIVerticalBox::OnDrawGui()
{
	ImGui::DragInt(u8"選択中インデックス", &_selectedIndex);
	ImGui::DragFloat(u8"高さ", &_height, 1.0f);
	ImGui::DragFloat(u8"余白", &_padding, 1.0f);
}
// 起動時
void UIVerticalBox::OnActivated()
{
	// 子要素を起動
	UIWidget::OnActivated();

	// 選択中の子ウィジェットを設定
	_selectedIndex = 0;
}
#pragma endregion

#pragma region UIButton
// 更新
void UIButton::OnUpdate(float elapsedTime)
{
}

// 描画
void UIButton::OnRender(Scene* scene)
{
}

// GUI描画
void UIButton::OnDrawGui()
{
}
#pragma endregion
