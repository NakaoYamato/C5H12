#include "TimerUIController.h"

#include "../../Library/Actor/UI/UIActor.h"

#include <imgui.h>

// 開始処理
void TimerUIController::Start()
{
    _spriteRenderer =  this->GetActor()->GetComponent<SpriteRenderer>();

    if (auto spriteRenderer = _spriteRenderer.lock())
    {
        if (!spriteRenderer->IsLoaded())
        {
            spriteRenderer->LoadTexture(FrameSpr, L"Data/Texture/UI/Timer/Frame.png");
            spriteRenderer->LoadTexture(HandSpr, L"Data/Texture/UI/Timer/Hand.png");
            spriteRenderer->LoadTexture(EndHandSpr, L"Data/Texture/UI/Timer/Hand.png");
        }

        _handRectTransform = &spriteRenderer->GetRectTransform(HandSpr);
    }

    // パラメータ設定
    if (auto uiActor = dynamic_cast<UIActor*>(this->GetActor().get()))
    {
        uiActor->GetRectTransform().SetLocalPosition(Vector2(1800.0f, 200.0f));
        uiActor->GetRectTransform().SetLocalScale(Vector2(0.15f, 0.15f));
    }
}

// 更新処理
void TimerUIController::Update(float elapsedTime)
{
    if (_handRectTransform)
    {
        // 60分で1週
        _handRectTransform->SetLocalAngle(
            DirectX::XMConvertToRadians(360.0f * _timer / 3600.0f)
        );
    }
}

// GUI描画
void TimerUIController::DrawGui()
{
	ImGui::DragFloat(u8"タイマー", &_timer, 1.0f, 0.0f, _endTimer);
	ImGui::DragFloat(u8"終了時間", &_endTimer, 10.0f, 0.0f, 3600.0f);
}

// 起動フラグが変化したときの処理
void TimerUIController::OnChangedActive(bool isActive)
{
    _timer = 0.0f;
}
