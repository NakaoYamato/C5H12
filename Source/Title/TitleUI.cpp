#include "TitleUI.h"

#include "../../Library/Scene/SceneManager.h"
#include "../../Library/Input/Input.h"

void TitleUI::Start()
{
    _spriteRenderer = GetActor()->GetComponent<SpriteRenderer>();
    _startButtonSize = _spriteRenderer.lock()->GetScale("StartButton");
}

void TitleUI::Update(float elapsedTime)
{
    Vector2 mousePos{};
    mousePos.x = _INPUT_IS_AXIS("MousePositionX");
    mousePos.y = _INPUT_IS_AXIS("MousePositionY");

    _spriteRenderer.lock()->SetScale("StartButton", _startButtonSize);
    if (_spriteRenderer.lock()->IsHit("StartButton", mousePos))
    {
        _spriteRenderer.lock()->SetScale("StartButton", _startButtonSize * 1.1f);

        if (_INPUT_IS_RELEASED("OK"))
        {
            SceneManager::Instance().ChangeScene(SceneMenuLevel::Game, "Game");
        }
    }
}

void TitleUI::DrawGui()
{
}
