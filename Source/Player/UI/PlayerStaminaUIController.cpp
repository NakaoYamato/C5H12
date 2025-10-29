#include "PlayerStaminaUIController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/InGame/CanvasMediator.h"

#include <imgui.h>

void PlayerStaminaUIController::Start()
{
	// キャンバスがあるか確認
	if (auto canvas = GetActor()->GetScene()->GetActorManager().FindByClass<CanvasMediator>(ActorTag::UI))
	{
		// 親設定
        GetActor()->SetParent(canvas.get());
	}

    _spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();

	if (auto spriteRenderer = _spriteRenderer.lock())
	{
		spriteRenderer->LoadTexture(FrameSpr, L"Data/Texture/UI/Player/Frame.png", Sprite::CenterAlignment::LeftCenter);
		spriteRenderer->LoadTexture(MaskSpr, L"Data/Texture/UI/Player/Mask.png", Sprite::CenterAlignment::LeftCenter);
		spriteRenderer->LoadTexture(GaugeSpr, L"Data/Texture/UI/Player/Mask.png", Sprite::CenterAlignment::LeftCenter);

        spriteRenderer->SetDepthState(MaskSpr, DepthState::SpriteMask);
		spriteRenderer->SetStencil(MaskSpr, 1);
        spriteRenderer->SetDepthState(GaugeSpr, DepthState::SpriteApplyMask);
		spriteRenderer->SetStencil(GaugeSpr, 1);
		spriteRenderer->SetColor(GaugeSpr, Vector4::Yellow);
    }
}

void PlayerStaminaUIController::Update(float elapsedTime)
{
	auto staminaController = _staminaController.lock();
	auto spriteRenderer = _spriteRenderer.lock();

	if (staminaController && spriteRenderer)
	{
        float rate = staminaController->GetStamina() / staminaController->GetMaxStamina();
        rate = std::clamp(rate, 0.0f, 1.0f);
		spriteRenderer->GetRectTransform(MaskSpr).SetLocalScale(Vector2(rate, 1.0f));
	}
}

void PlayerStaminaUIController::DrawGui()
{
}
