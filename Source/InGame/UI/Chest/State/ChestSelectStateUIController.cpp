#include "ChestSelectStateUIController.h"

#include "../../Library/Actor/UI/UIActor.h"
#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// ŠJŽnˆ—
void ChestSelectStateUIController::Start()
{
    _spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();

    if (auto spriteRenderer = _spriteRenderer.lock())
    {
        spriteRenderer->LoadTexture(ItemBackSpr, L"Data/Texture/UI/Chest/Back.png");
        spriteRenderer->LoadTexture(ArmorBackSpr, L"Data/Texture/UI/Chest/Back.png");
        spriteRenderer->LoadTexture(TestBackSpr, L"Data/Texture/UI/Chest/Back.png");
        spriteRenderer->LoadTexture(FrontSpr, L"Data/Texture/UI/Chest/Front.png");

        spriteRenderer->GetRectTransform(ItemBackSpr).SetLocalScale(Vector2(0.5f, 0.4f));
        spriteRenderer->SetColor(ItemBackSpr, Vector4(1.0f, 1.0f, 1.0f, 0.6f));

        spriteRenderer->GetRectTransform(ArmorBackSpr).SetLocalScale(Vector2(0.5f, 0.4f));
        spriteRenderer->GetRectTransform(ArmorBackSpr).SetLocalPosition(Vector2(0.0f, BackPositionInterval));
        spriteRenderer->SetColor(ArmorBackSpr, Vector4(1.0f, 1.0f, 1.0f, 0.6f));

        spriteRenderer->GetRectTransform(TestBackSpr).SetLocalScale(Vector2(0.5f, 0.4f));
        spriteRenderer->GetRectTransform(TestBackSpr).SetLocalPosition(Vector2(0.0f, BackPositionInterval * 2.0f));
        spriteRenderer->SetColor(TestBackSpr, Vector4(1.0f, 1.0f, 1.0f, 0.6f));

        spriteRenderer->GetRectTransform(FrontSpr).SetLocalScale(Vector2(0.5f, 0.4f));
        spriteRenderer->SetColor(FrontSpr, Vector4(1.0f, 1.0f, 1.0f, 0.8f));
    }
}

// XVˆ—
void ChestSelectStateUIController::Update(float elapsedTime)
{
}

// 3D•`‰æŒã‚Ì•`‰æˆ—
void ChestSelectStateUIController::DelayedRender(const RenderContext& rc)
{
    auto spriteRenderer = _spriteRenderer.lock();
    if (!spriteRenderer)
        return;
    auto& textRenderer = GetActor()->GetScene()->GetTextRenderer();

    // Še”wŒi‚É•`‰æ
    textRenderer.Draw(FontType::MSGothic,
        "ƒAƒCƒeƒ€",
        spriteRenderer->GetRectTransform(ItemBackSpr).GetWorldPosition() + _textOffset,
        Vector4::White,
        0.0f,
        _textOrigin,
        _textScale);
    textRenderer.Draw(FontType::MSGothic,
        "–h‹ï",
        spriteRenderer->GetRectTransform(ArmorBackSpr).GetWorldPosition() + _textOffset,
        Vector4::White,
        0.0f,
        _textOrigin,
        _textScale);
    textRenderer.Draw(FontType::MSGothic,
        "ƒeƒXƒg",
        spriteRenderer->GetRectTransform(TestBackSpr).GetWorldPosition() + _textOffset,
        Vector4::White,
        0.0f,
        _textOrigin,
        _textScale);
}

// GUI•`‰æ
void ChestSelectStateUIController::DrawGui()
{
}
