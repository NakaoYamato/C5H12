#include "PlayerStaminaUIController.h"

#include "../../Library/Scene/Scene.h"

#include <imgui.h>

void PlayerStaminaUIController::Start()
{
    _spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();

	if (auto spriteRenderer = _spriteRenderer.lock())
	{
		if (!spriteRenderer->IsLoaded())
		{
			// 画像読み込み
			spriteRenderer->LoadTexture(FrameSpr, L"Data/Texture/UI/Player/Frame.png", Sprite::CenterAlignment::LeftCenter);
			spriteRenderer->LoadTexture(MaskSpr, L"", Sprite::CenterAlignment::LeftCenter);
			spriteRenderer->LoadTexture(GaugeSpr, L"Data/Texture/UI/Player/Mask.png", Sprite::CenterAlignment::LeftCenter);
			spriteRenderer->LoadTexture(GaugeEndSprite, L"Data/Texture/UI/Player/GaugeEnd.png", Sprite::CenterAlignment::CenterCenter);
		}
    }
}

void PlayerStaminaUIController::Update(float elapsedTime)
{
	auto staminaController = _staminaController.lock();
	auto spriteRenderer = _spriteRenderer.lock();

	if (staminaController && spriteRenderer)
	{
		// スタミナに応じてスケールを変更
        float rate = staminaController->GetStamina() / staminaController->GetMaxStamina();
        rate = std::clamp(rate, 0.0f, 1.0f);
		spriteRenderer->GetRectTransform(MaskSpr).SetLocalScale(Vector2(_maskStartScaleX * rate, 1.0f));

		// 先端設定
		float posX = MathF::Lerp(_gaugeEndEndX, _gaugeEndStartX, rate);
		spriteRenderer->GetRectTransform(GaugeEndSprite).SetLocalPosition(Vector2(posX, 1.0f));
		if (rate >= 1.0f)
		{
			spriteRenderer->GetRectTransform(GaugeEndSprite).SetLocalScale(Vector2(0.0f, 0.0f));
		}
		else
		{
			spriteRenderer->GetRectTransform(GaugeEndSprite).SetLocalScale(Vector2(2.0f, 4.0f));
		}
	}
}

void PlayerStaminaUIController::DrawGui()
{
}
