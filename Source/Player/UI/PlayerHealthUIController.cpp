#include "PlayerHealthUIController.h"

#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// 開始処理
void PlayerHealthUIController::Start()
{
	// シーンからCanvasMediatorを取得
	_canvasMediator = GetActor()->GetScene()->GetActorManager().FindByClass<CanvasMediator>(ActorTag::UI);
	if (_canvasMediator.lock())
	{
		// 親設定
		GetActor()->SetParent(_canvasMediator.lock().get());
		// ユーザーが操作するプレイヤーのHPUIの場合は、CanvasMediatorに登録
		if (_isUserControlled)
			_canvasMediator.lock()->SetUserHealthUI(this);
		// ユーザーが操作しないプレイヤーのHPUIの場合は、CanvasMediatorに登録
		else
			_canvasMediator.lock()->AddOtherUserHealthUI(this);
    }
	// 画像読み込み
	LoadTexture(FrameSprite, L"Data/Texture/UI/Player/Frame.png", Sprite::CenterAlignment::LeftCenter);
	LoadTexture(MaskSprite, L"", Sprite::CenterAlignment::LeftCenter);
	LoadTexture(GaugeSprite, L"Data/Texture/UI/Player/Mask.png", Sprite::CenterAlignment::LeftCenter);
	LoadTexture(DamageGaugeMaskSprite, L"", Sprite::CenterAlignment::LeftCenter);
	LoadTexture(DamageGaugeSprite, L"Data/Texture/UI/Player/Mask.png", Sprite::CenterAlignment::LeftCenter);
	LoadTexture(GaugeEndSprite, L"Data/Texture/UI/Player/GaugeEnd.png", Sprite::CenterAlignment::CenterCenter);

	SetDepthState(MaskSprite, DepthState::SpriteMask);
	SetStencil(MaskSprite, 1);
	GetRectTransform(MaskSprite).SetLocalPosition(Vector2(_maskStartPosX, 0.0f));
	GetRectTransform(MaskSprite).SetLocalScale(Vector2(_maskStartScaleX, 1.0f));

	SetDepthState(GaugeSprite, DepthState::SpriteApplyMask);
	SetStencil(GaugeSprite, 1);
    SetColor(GaugeSprite, Vector4::Green);

	SetDepthState(DamageGaugeMaskSprite, DepthState::SpriteMask);
	SetStencil(DamageGaugeMaskSprite, 2);
	GetRectTransform(DamageGaugeMaskSprite).SetLocalPosition(Vector2(_maskStartPosX, 0.0f));
	GetRectTransform(DamageGaugeMaskSprite).SetLocalScale(Vector2(_maskStartScaleX, 1.0f));

	SetDepthState(DamageGaugeSprite, DepthState::SpriteApplyMask);
	SetStencil(DamageGaugeSprite, 2);
    SetColor(DamageGaugeSprite, Vector4::Red);

	GetRectTransform(GaugeEndSprite).SetLocalScale(Vector2(2.0f, 4.0f));
	SetColor(GaugeEndSprite, Vector4::Green);
}
// 削除処理
void PlayerHealthUIController::OnDelete()
{
	if (_canvasMediator.lock())
	{
		// ユーザーが操作しないプレイヤーのHPUIの場合は、CanvasMediatorから削除
		if (!_isUserControlled)
		{
			_canvasMediator.lock()->RemoveOtherUserHealthUI(this);
		}
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
	ImGui::Text(u8"キャンバス: %s", _canvasMediator.lock() ? u8"有効" : u8"無効");
	ImGui::DragFloat(u8"ダメージゲージスケール速度", &_damageGaugeScaleSpeed, 0.1f, 0.0f, 10.0f);
	UIController::DrawGui();
}

void PlayerHealthUIController::DrawUI(const RenderContext& rc)
{
	const RenderState* renderState = rc.renderState;
	SpriteRender(FrameSprite, rc);

	SpriteRender(DamageGaugeMaskSprite, rc);
	SpriteRender(DamageGaugeSprite, rc);

	SpriteRender(MaskSprite, rc);
	SpriteRender(GaugeSprite, rc);
	
	rc.deviceContext->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	SpriteRender(GaugeEndSprite, rc);
}
