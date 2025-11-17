#include "ItemSliderController.h"

#include <imgui.h>

// 開始処理
void ItemSliderController::Start()
{
    _spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();

    if (auto spriteRenderer = _spriteRenderer.lock())
    {
        spriteRenderer->SetOverallAlpha(0.0f);

        spriteRenderer->LoadTexture(LeftUnderlineSpr, L"Data/Texture/UI/Item/ItemSlideUnderline.png");
        spriteRenderer->LoadTexture(RightUnderlineSpr, L"Data/Texture/UI/Item/ItemSlideUnderline.png");

        spriteRenderer->GetRectTransform(LeftUnderlineSpr).SetLocalPosition(Vector2(-125.0f, 50.0f));
        spriteRenderer->GetRectTransform(LeftUnderlineSpr).SetLocalScale(Vector2(0.7f, 0.4f));
        spriteRenderer->GetRectTransform(RightUnderlineSpr).SetLocalPosition(Vector2(125.0f, 50.0f));
        spriteRenderer->GetRectTransform(RightUnderlineSpr).SetLocalScale(Vector2(0.7f, 0.4f));
        spriteRenderer->GetRectTransform(RightUnderlineSpr).SetLocalAngle(DirectX::XMConvertToRadians(180.0f));
    }
}

// 更新処理
void ItemSliderController::Update(float elapsedTime)
{
    if (auto spriteRenderer = _spriteRenderer.lock())
    {
        if (spriteRenderer->GetOverallAlpha() < 1.0f)
        {
            spriteRenderer->SetOverallAlpha(MathF::Lerp(spriteRenderer->GetOverallAlpha(), 1.0f, _alphaChangeSpeed * elapsedTime));
        }
    }
}

// GUI描画
void ItemSliderController::DrawGui()
{
	ImGui::DragFloat(u8"全体透明度変更速度", &_alphaChangeSpeed, 0.1f, 0.1f, 10.0f);
}

// 起動フラグが変化したときの処理
void ItemSliderController::OnChangedActive(bool isActive)
{
	// 透明度変更
	if (auto spriteRenderer = _spriteRenderer.lock())
	{
		spriteRenderer->SetOverallAlpha(0.0f);
	}
}
