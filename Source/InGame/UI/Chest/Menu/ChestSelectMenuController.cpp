#include "ChestSelectMenuController.h"

#include "../ChestUIController.h"
#include "../../Library/Actor/UI/UIActor.h"

#include <imgui.h>

// 開始処理
void ChestSelectMenuController::Start()
{
    _spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();

    if (auto spriteRenderer = _spriteRenderer.lock())
    {
        if (!spriteRenderer->IsLoaded())
        {
            spriteRenderer->LoadTexture(ItemBackSpr, L"Data/Texture/UI/Chest/Back.png");
            spriteRenderer->LoadTexture(ArmorBackSpr, L"Data/Texture/UI/Chest/Back.png");
            spriteRenderer->LoadTexture(FrontSpr, L"Data/Texture/UI/Chest/Front.png");
        }
    }

    // 入力UI
    _selectInputUI.keyboardKey = 'F';
    _selectInputUI.gamePadKey = XINPUT_GAMEPAD_A;
    _selectInputUI.color = Vector4::White;
    _selectInputUI.position = Vector2(110.0f, 960.0f);
    _selectInputUI.scale = Vector2(0.7f, 0.7f);
    _selectInputUIText.text = L"：選択";
    _selectInputUIText.position = Vector2(150.0f, 940.0f);
    _backInputUI.keyboardKey = VK_ESCAPE;
    _backInputUI.gamePadKey = XINPUT_GAMEPAD_B;
    _backInputUI.color = Vector4::White;
    _backInputUI.position = Vector2(470.0f, 960.0f);
    _backInputUI.scale = Vector2(0.7f, 0.7f);
    _backInputUIText.text = L"：戻る";
    _backInputUIText.position = Vector2(500.0f, 940.0f);
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

    // 入力UI
    GetActor()->GetScene()->GetInputUI()->Draw(_selectInputUI);;
    GetActor()->GetScene()->GetTextRenderer().Draw(_selectInputUIText);
    GetActor()->GetScene()->GetInputUI()->Draw(_backInputUI);;
    GetActor()->GetScene()->GetTextRenderer().Draw(_backInputUIText);
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

void ChestSelectMenuController::AddIndex(int direction)
{
	if (direction & static_cast<unsigned int>(ChestUIController::InputDirection::Up))
        _index = static_cast<SelectMenuOption>(static_cast<int>(_index) - 1);
	else if (direction & static_cast<unsigned int>(ChestUIController::InputDirection::Down))
        _index = static_cast<SelectMenuOption>(static_cast<int>(_index) + 1);
    else
		return;

    // indexの制限
    if (_index < SelectMenuOption::ItemOption)
        _index = SelectMenuOption::ArmorOption;
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
    }

    spriteRenderer->GetRectTransform(FrontSpr).SetLocalPosition(position);
}

// リセット
void ChestSelectMenuController::Reset()
{
    _index = SelectMenuOption::ItemOption;
	if (auto spriteRenderer = _spriteRenderer.lock())
	{
		// 背景位置更新
		Vector2 position = spriteRenderer->GetRectTransform(ItemBackSpr).GetLocalPosition();
		spriteRenderer->GetRectTransform(FrontSpr).SetLocalPosition(position);
	}
}
