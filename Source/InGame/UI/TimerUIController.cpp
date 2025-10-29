#include "TimerUIController.h"

#include "../../Library/Actor/UI/UIActor.h"

#include <imgui.h>

void TimerUIController::Start()
{
    _spriteRenderer =  this->GetActor()->GetComponent<SpriteRenderer>();

    if (auto spriteRenderer = _spriteRenderer.lock())
    {
        spriteRenderer->LoadTexture(FrameSpr, L"Data/Texture/UI/Timer/Frame.png");
        spriteRenderer->LoadTexture(HandSpr, L"Data/Texture/UI/Timer/Hand.png");
        spriteRenderer->LoadTexture(EndHandSpr, L"Data/Texture/UI/Timer/Hand.png");

        spriteRenderer->SetColor(HandSpr, Vector4::Black);
        spriteRenderer->SetColor(EndHandSpr, Vector4::Red);

        _handRectTransform = &spriteRenderer->GetRectTransform(HandSpr);

        // 60ï™Ç≈1èT
        spriteRenderer->GetRectTransform(EndHandSpr).SetLocalAngle(
            DirectX::XMConvertToRadians(360.0f * _endTimer / 3600.0f)
        );
    }

    // ÉpÉâÉÅÅ[É^ê›íË
    if (auto uiActor = dynamic_cast<UIActor*>(this->GetActor().get()))
    {
        uiActor->GetRectTransform().SetLocalPosition(Vector2(1800.0f, 300.0f));
        uiActor->GetRectTransform().SetLocalScale(Vector2(0.15f, 0.15f));
    }
}

void TimerUIController::Update(float elapsedTime)
{
    _timer += elapsedTime;
    if (_handRectTransform)
    {
        // 60ï™Ç≈1èT
        _handRectTransform->SetLocalAngle(
            DirectX::XMConvertToRadians(360.0f * _timer / 3600.0f)
        );
    }
}

void TimerUIController::DrawGui()
{
}
