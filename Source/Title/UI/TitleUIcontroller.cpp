#include "TitleUIcontroller.h"

#include "../../Library/Actor/UI/UIActor.h"
#include "../../Library/Algorithm/Converter.h"
#include "../../Source/Scene/Game/SceneGame.h"
#include "../../Library/Graphics/Graphics.h"

#include <imgui.h>
// 開始処理
void TitleUIcontroller::Start()
{
    auto uiActor = std::dynamic_pointer_cast<UIActor>(GetActor());
    if (uiActor)
    {
        _myRectTransform = &uiActor->GetRectTransform();
    }

    _spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();
    if (auto spriteRenderer = _spriteRenderer.lock())
    {
        if (!spriteRenderer->IsLoaded())
        {
            spriteRenderer->LoadTexture(ToGameSpr, L"Data/Texture/Title/Back.png");
            spriteRenderer->LoadTexture(ToPlayerDebugSpr, L"Data/Texture/Title/Back.png");
            spriteRenderer->LoadTexture(QuitSpr, L"Data/Texture/Title/Back.png");
            spriteRenderer->LoadTexture(FrontSpr, L"Data/Texture/Title/Front.png");
        }
    }

    _textData.scale = Vector2(1.3f, 1.3f);
}

// 更新処理
void TitleUIcontroller::Update(float elapsedTime)
{
    auto spriteRenderer = _spriteRenderer.lock();
    if (!spriteRenderer)
        return;

    // 入力処理
    if (!GetActor()->GetScene()->GetFade()->IsFading())
    {
        if (_INPUT_TRIGGERD("Up"))
        {
            _selectType = static_cast<SelectType>(
                (static_cast<unsigned int>(_selectType) + 2) % 3
                );
        }
        if (_INPUT_TRIGGERD("Down"))
        {
            _selectType = static_cast<SelectType>(
                (static_cast<unsigned int>(_selectType) + 1) % 3
                );
        }
        if (_INPUT_RELEASED("Select"))
        {
            switch (_selectType)
            {
            case TitleUIcontroller::SelectType::ToGame:
                SceneGame::NetworkEnabled = false;
                SceneManager::Instance().ChangeScene(SceneMenuLevel::Game, "Game");
                return;
            case TitleUIcontroller::SelectType::ToPlayerDebug:
                SceneManager::Instance().ChangeScene(SceneMenuLevel::Debug, "PlayerDebug");
                return;
            case TitleUIcontroller::SelectType::Quit:
                PostMessage(Graphics::Instance().GetHwnd(), WM_CLOSE, 0, 0);
                return;
            }
        }
    }

    // 前面を選択中のスプライトの上に移動
    switch (_selectType)
    {
    case TitleUIcontroller::SelectType::ToGame:
        spriteRenderer->GetRectTransform(FrontSpr).SetLocalPosition(
            spriteRenderer->GetRectTransform(ToGameSpr).GetWorldPosition()
        );
        break;
    case TitleUIcontroller::SelectType::ToPlayerDebug:
        spriteRenderer->GetRectTransform(FrontSpr).SetLocalPosition(
            spriteRenderer->GetRectTransform(ToPlayerDebugSpr).GetWorldPosition()
        );
        break;
    case TitleUIcontroller::SelectType::Quit:
        spriteRenderer->GetRectTransform(FrontSpr).SetLocalPosition(
            spriteRenderer->GetRectTransform(QuitSpr).GetWorldPosition()
        );
        break;
    }

    // 文字描画
    _textData.text = L"ゲーム開始";
    _textData.position = spriteRenderer->GetRectTransform(ToGameSpr).GetWorldPosition() + _textOffset;
    GetActor()->GetScene()->GetTextRenderer().Draw(_textData);
    _textData.text = L"プレイヤーデバッグ";
    _textData.position = spriteRenderer->GetRectTransform(ToPlayerDebugSpr).GetWorldPosition() + _textOffset;
    GetActor()->GetScene()->GetTextRenderer().Draw(_textData);
    _textData.text = L"終了";
    _textData.position = spriteRenderer->GetRectTransform(QuitSpr).GetWorldPosition() + _textOffset;
    GetActor()->GetScene()->GetTextRenderer().Draw(_textData);

	// 入力UI描画
	InputUI::DrawInfo drawInfo;
	drawInfo.keyboardKey = 'F';
	drawInfo.gamePadKey = XINPUT_GAMEPAD_A;
    drawInfo.isActive = _INPUT_PRESSED("Select");
    switch (_selectType)
    {
    case TitleUIcontroller::SelectType::ToGame:
        drawInfo.position = spriteRenderer->GetRectTransform(ToGameSpr).GetWorldPosition() + _inputUIOffset;
        break;
    case TitleUIcontroller::SelectType::ToPlayerDebug:
        drawInfo.position = spriteRenderer->GetRectTransform(ToPlayerDebugSpr).GetWorldPosition() + _inputUIOffset;
        break;
    case TitleUIcontroller::SelectType::Quit:
        drawInfo.position = spriteRenderer->GetRectTransform(QuitSpr).GetWorldPosition() + _inputUIOffset;
        break;
    }
	drawInfo.scale = Vector2::One;
	drawInfo.color = Vector4::BlueGreen;
	GetActor()->GetScene()->GetInputUI()->Draw(drawInfo);
}

// GUI描画
void TitleUIcontroller::DrawGui()
{
    _textData.DrawGui(u8"テキスト");
    ImGui::DragFloat2(u8"テキストオフセット", &_textOffset.x, 0.1f);
    ImGui::DragFloat2(u8"入力UIオフセット", &_inputUIOffset.x, 0.1f);
}
