#include "PlayerHealthUIController.h"

#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// 開始処理
void PlayerHealthUIController::Start()
{
	// シーンからInGameCanvasActorを取得
	if (auto canvas = GetActor()->GetScene()->GetActorManager().FindByClass<InGameCanvasActor>(ActorTag::UI))
	{
		// 親設定
		GetActor()->SetParent(canvas.get());
    }
	if (!IsLoaded())
	{
		// 画像読み込み
		LoadTexture(FrameSprite, L"Data/Texture/UI/Player/Frame.png", Sprite::CenterAlignment::LeftCenter);
		LoadTexture(DamageGaugeMaskSprite, L"", Sprite::CenterAlignment::LeftCenter);
		LoadTexture(DamageGaugeSprite, L"Data/Texture/UI/Player/Mask.png", Sprite::CenterAlignment::LeftCenter);
		LoadTexture(MaskSprite, L"", Sprite::CenterAlignment::LeftCenter);
		LoadTexture(GaugeSprite, L"Data/Texture/UI/Player/Mask.png", Sprite::CenterAlignment::LeftCenter);
		LoadTexture(GaugeEndSprite, L"Data/Texture/UI/Player/GaugeEnd.png", Sprite::CenterAlignment::CenterCenter);
	}
}
// 更新処理
void PlayerHealthUIController::Update(float elapsedTime)
{
	SpriteRenderer::Update(elapsedTime);

	// HPに応じてスケールを変更
	if (_damageable.lock())
	{
		float healthRatio = _damageable.lock()->GetHealth() / _damageable.lock()->GetMaxHealth();
		healthRatio = std::clamp(healthRatio, 0.0f, 1.0f); // 0.0fから1.0fの範囲に制限
		GetRectTransform(MaskSprite).SetLocalScale(Vector2(_maskStartScaleX * healthRatio, 1.0f)); // HPに応じて横幅を変更

		// ダメージゲージのスケールを更新
		float damageGaugeScaleX = EasingLerp(
			GetRectTransform(DamageGaugeMaskSprite).GetWorldScale().x,
			GetRectTransform(MaskSprite).GetWorldScale().x,
			_damageGaugeScaleSpeed * elapsedTime);
		GetRectTransform(DamageGaugeMaskSprite).SetLocalScale(Vector2(damageGaugeScaleX, 1.0f)); // ダメージゲージの横幅を変更

		// 先端設定
		float posX = MathF::Lerp(_gaugeEndEndX, _gaugeEndStartX, healthRatio);
		GetRectTransform(GaugeEndSprite).SetLocalPosition(Vector2(posX, 1.0f));

		if (healthRatio >= 1.0f)
		{
			GetRectTransform(GaugeEndSprite).SetLocalScale(Vector2(0.0f, 0.0f));
		}
		else
		{
			GetRectTransform(GaugeEndSprite).SetLocalScale(Vector2(2.0f, 4.0f));
		}
	}
}
// GUI描画
void PlayerHealthUIController::DrawGui()
{
	ImGui::DragFloat(u8"ダメージゲージスケール速度", &_damageGaugeScaleSpeed, 0.1f, 0.0f, 10.0f);
	ImGui::Separator();
	SpriteRenderer::DrawGui();
}
