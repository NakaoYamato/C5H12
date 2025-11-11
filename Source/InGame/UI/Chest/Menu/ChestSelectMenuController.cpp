#include "ChestSelectMenuController.h"

#include "../../Library/Actor/UI/UIActor.h"
#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// 開始処理
void ChestSelectMenuController::Start()
{
    _spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();

    if (auto spriteRenderer = _spriteRenderer.lock())
    {
        spriteRenderer->LoadTexture(ItemBackSpr, L"Data/Texture/UI/Chest/Back.png");
        spriteRenderer->LoadTexture(ArmorBackSpr, L"Data/Texture/UI/Chest/Back.png");
        spriteRenderer->LoadTexture(TestBackSpr, L"Data/Texture/UI/Chest/Back.png");
        spriteRenderer->LoadTexture(FrontSpr, L"Data/Texture/UI/Chest/Front.png");

        spriteRenderer->GetRectTransform(ItemBackSpr).SetLocalScale(Vector2(0.5f, 0.4f));
		spriteRenderer->GetRectTransform(ItemBackSpr).SetLocalPosition(SprStartPos);
        spriteRenderer->SetColorAlpha(ItemBackSpr, 0.6f);

        spriteRenderer->GetRectTransform(ArmorBackSpr).SetLocalScale(Vector2(0.5f, 0.4f));
        spriteRenderer->GetRectTransform(ArmorBackSpr).SetLocalPosition(SprStartPos + Vector2(0.0f, BackPositionInterval));
        spriteRenderer->SetColorAlpha(ArmorBackSpr, 0.6f);

        spriteRenderer->GetRectTransform(TestBackSpr).SetLocalScale(Vector2(0.5f, 0.4f));
        spriteRenderer->GetRectTransform(TestBackSpr).SetLocalPosition(SprStartPos + Vector2(0.0f, BackPositionInterval * 2.0f));
        spriteRenderer->SetColorAlpha(TestBackSpr, 0.6f);

        spriteRenderer->GetRectTransform(FrontSpr).SetLocalScale(Vector2(0.5f, 0.4f));
        spriteRenderer->SetColorAlpha(FrontSpr, 0.8f);
    }
}

// 3D描画後の描画処理
void ChestSelectMenuController::DelayedRender(const RenderContext& rc)
{
    auto spriteRenderer = _spriteRenderer.lock();
    if (!spriteRenderer)
        return;
    auto& textRenderer = GetActor()->GetScene()->GetTextRenderer();

    // 各背景に描画
    textRenderer.Draw(FontType::MSGothic,
        "アイテム",
        spriteRenderer->GetRectTransform(ItemBackSpr).GetWorldPosition() + _textOffset,
        _index == SelectMenuOption::ItemOption ? _textSelectColor : _textColor,
        0.0f,
        _textOrigin,
        _textScale);
    textRenderer.Draw(FontType::MSGothic,
        "防具",
        spriteRenderer->GetRectTransform(ArmorBackSpr).GetWorldPosition() + _textOffset,
        _index == SelectMenuOption::ArmorOption ? _textSelectColor : _textColor,
        0.0f,
        _textOrigin,
        _textScale);
    textRenderer.Draw(FontType::MSGothic,
        "テスト",
        spriteRenderer->GetRectTransform(TestBackSpr).GetWorldPosition() + _textOffset,
        _index == SelectMenuOption::TestOption ? _textSelectColor : _textColor,
        0.0f,
        _textOrigin,
        _textScale);
}

// GUI描画
void ChestSelectMenuController::DrawGui()
{
	ImGui::DragFloat2(u8"テキストオフセット", &_textOffset.x, 1.0f);
	ImGui::DragFloat2(u8"テキスト原点位置", &_textOrigin.x, 0.1f);
	ImGui::DragFloat2(u8"テキストスケール", &_textScale.x, 0.1f);
	ImGui::ColorEdit4(u8"テキスト通常色", &_textColor.x);
	ImGui::ColorEdit4(u8"テキスト選択色", &_textSelectColor.x);
}

void ChestSelectMenuController::AddIndex(int val)
{
    _index = static_cast<SelectMenuOption>(static_cast<int>(_index) + val);

    // indexの制限
    if (_index < SelectMenuOption::ItemOption)
        _index = SelectMenuOption::TestOption;
    else if (_index >= SelectMenuOption::MaxOption)
        _index = SelectMenuOption::ItemOption;

    auto spriteRenderer = _spriteRenderer.lock();
    if (!spriteRenderer)
        return;

    // 背景位置更新
    Vector2 position{};
    switch (_index)
    {
    case ChestSelectMenuController::SelectMenuOption::ItemOption:
		position = spriteRenderer->GetRectTransform(ItemBackSpr).GetLocalPosition();
        break;
    case ChestSelectMenuController::SelectMenuOption::ArmorOption:
		position = spriteRenderer->GetRectTransform(ArmorBackSpr).GetLocalPosition();
        break;
    case ChestSelectMenuController::SelectMenuOption::TestOption:
		position = spriteRenderer->GetRectTransform(TestBackSpr).GetLocalPosition();
        break;
    }

    spriteRenderer->GetRectTransform(FrontSpr).SetLocalPosition(position);
}

// リセット
void ChestSelectMenuController::ResetIndex()
{
    _index = SelectMenuOption::ItemOption;
	if (auto spriteRenderer = _spriteRenderer.lock())
	{
		// 背景位置更新
		Vector2 position = spriteRenderer->GetRectTransform(ItemBackSpr).GetLocalPosition();
		spriteRenderer->GetRectTransform(FrontSpr).SetLocalPosition(position);
	}
}
