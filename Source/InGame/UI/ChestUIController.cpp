#include "ChestUIController.h"

#include "../../Library/Actor/UI/UIActor.h"

#include <imgui.h>

// 開始処理
void ChestUIController::Start()
{
    _spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();

    if (auto spriteRenderer = _spriteRenderer.lock())
    {
        spriteRenderer->LoadTexture(ItemBackSpr, L"Data/Texture/UI/Chest/Back.png");
        spriteRenderer->LoadTexture(ArmorBackSpr, L"Data/Texture/UI/Chest/Back.png");

        spriteRenderer->GetRectTransform(ItemBackSpr).SetLocalScale(Vector2(0.5f, 0.4f));
		spriteRenderer->SetColor(ItemBackSpr, Vector4(1.0f, 1.0f, 1.0f, 0.8f));

        spriteRenderer->GetRectTransform(ArmorBackSpr).SetLocalScale(Vector2(0.5f, 0.4f));
        spriteRenderer->GetRectTransform(ArmorBackSpr).SetLocalPosition(Vector2(1.0f, 120.0f));
        spriteRenderer->SetColor(ArmorBackSpr, Vector4(1.0f, 1.0f, 1.0f, 0.8f));
    }
}

// 更新処理
void ChestUIController::Update(float elapsedTime)
{
}

// GUI描画
void ChestUIController::DrawGui()
{
}
