#include "ItemSliderController.h"

// 開始処理
void ItemSliderController::Start()
{
    _spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();

    if (auto spriteRenderer = _spriteRenderer.lock())
    {
        spriteRenderer->LoadTexture(LeftUnderlineSpr, L"Data/Texture/UI/Item/ItemSlideUnderline.png");
        spriteRenderer->LoadTexture(RightUnderlineSpr, L"Data/Texture/UI/Item/ItemSlideUnderline.png");

        spriteRenderer->GetRectTransform(LeftUnderlineSpr).SetLocalPosition(Vector2(-125.0f, 0.0f));
        spriteRenderer->GetRectTransform(LeftUnderlineSpr).SetLocalScale(Vector2(0.7f, 0.4f));
        spriteRenderer->GetRectTransform(RightUnderlineSpr).SetLocalPosition(Vector2(125.0f, 0.0f));
        spriteRenderer->GetRectTransform(RightUnderlineSpr).SetLocalScale(Vector2(0.7f, 0.4f));
        spriteRenderer->GetRectTransform(RightUnderlineSpr).SetLocalAngle(DirectX::XMConvertToRadians(180.0f));
    }
}

// 更新処理
void ItemSliderController::Update(float elapsedTime)
{
}

// GUI描画
void ItemSliderController::DrawGui()
{
}
