#include "PlayerHealthUIController.h"

#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// 開始処理
void PlayerHealthUIController::Start()
{
    _spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();
	if (auto spriteRenderer = _spriteRenderer.lock())
	{
		if (!spriteRenderer->IsLoaded())
		{
			// 画像読み込み
			spriteRenderer->LoadTexture(FrameSprite, L"Data/Texture/UI/Player/Frame.png", Sprite::CenterAlignment::LeftCenter);
			spriteRenderer->LoadTexture(DamageGaugeMaskSprite, L"", Sprite::CenterAlignment::LeftCenter);
			spriteRenderer->LoadTexture(DamageGaugeSprite, L"Data/Texture/UI/Player/Mask.png", Sprite::CenterAlignment::LeftCenter);
			spriteRenderer->LoadTexture(MaskSprite, L"", Sprite::CenterAlignment::LeftCenter);
			spriteRenderer->LoadTexture(GaugeSprite, L"Data/Texture/UI/Player/Mask.png", Sprite::CenterAlignment::LeftCenter);
			spriteRenderer->LoadTexture(GaugeEndSprite, L"Data/Texture/UI/Player/GaugeEnd.png", Sprite::CenterAlignment::CenterCenter);
		}
	}
}
// 更新処理
void PlayerHealthUIController::Update(float elapsedTime)
{
    auto spriteRenderer = _spriteRenderer.lock();
	if (!spriteRenderer)
        return;

	// HPに応じてスケールを変更
	if (_damageable.lock())
	{
		float healthRatio = _damageable.lock()->GetHealth() / _damageable.lock()->GetMaxHealth();
		healthRatio = std::clamp(healthRatio, 0.0f, 1.0f); // 0.0fから1.0fの範囲に制限
		spriteRenderer->GetRectTransform(MaskSprite).SetLocalScale(Vector2(_maskStartScaleX * healthRatio, 1.0f)); // HPに応じて横幅を変更

		// ダメージゲージのスケールを更新
		float damageGaugeScaleX = EasingLerp(
			spriteRenderer->GetRectTransform(DamageGaugeMaskSprite).GetWorldScale().x,
			spriteRenderer->GetRectTransform(MaskSprite).GetWorldScale().x,
			_damageGaugeScaleSpeed * elapsedTime);
		spriteRenderer->GetRectTransform(DamageGaugeMaskSprite).SetLocalScale(Vector2(damageGaugeScaleX, 1.0f)); // ダメージゲージの横幅を変更

		// 先端設定
		float posX = MathF::Lerp(_gaugeEndEndX, _gaugeEndStartX, healthRatio);
		spriteRenderer->GetRectTransform(GaugeEndSprite).SetLocalPosition(Vector2(posX, 1.0f));

		if (healthRatio >= 1.0f)
		{
			spriteRenderer->GetRectTransform(GaugeEndSprite).SetLocalScale(Vector2(0.0f, 0.0f));
		}
		else
		{
			spriteRenderer->GetRectTransform(GaugeEndSprite).SetLocalScale(Vector2(2.0f, 4.0f));
		}
	}
}
// GUI描画
void PlayerHealthUIController::DrawGui()
{
	ImGui::DragFloat(u8"ダメージゲージスケール速度", &_damageGaugeScaleSpeed, 0.1f, 0.0f, 10.0f);
}
