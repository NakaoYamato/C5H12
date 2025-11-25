#include "TitleUIcontroller.h"

#include "../../Library/Actor/UI/UIActor.h"
#include "../../Library/Algorithm/Converter.h"
#include "../../Source/Scene/Game/SceneGame.h"

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
        spriteRenderer->LoadTexture(BoxSpr, L"Data/Texture/Title/Start.png");
        spriteRenderer->GetRectTransform(BoxSpr).SetLocalPosition(Vector2(1400.0f, 800.0f));
        spriteRenderer->GetRectTransform(BoxSpr).SetLocalScale(Vector2(2.0f, 2.0f));
    }
}

// 更新処理
void TitleUIcontroller::Update(float elapsedTime)
{
    auto spriteRenderer = _spriteRenderer.lock();
    if (!spriteRenderer)
        return;

	// 入力UI描画
	InputUI::DrawInfo drawInfo;
	drawInfo.keyboardKey = 'F';
	drawInfo.gamePadKey = XINPUT_GAMEPAD_A;
    drawInfo.isActive = _INPUT_PRESSED("Select");
	drawInfo.position = spriteRenderer->GetRectTransform(BoxSpr).GetWorldPosition() + _textOffset;
	drawInfo.scale = Vector2::One;
	drawInfo.color = Vector4::BlueGreen;
	GetActor()->GetScene()->GetInputUI()->Draw(drawInfo);

    if (!GetActor()->GetScene()->GetFade()->IsFading())
    {
        if (_INPUT_RELEASED("Select"))
        {
            SceneGame::NetworkEnabled = false;
            SceneManager::Instance().ChangeScene(SceneMenuLevel::Game, "Game");
        }
    }
}
