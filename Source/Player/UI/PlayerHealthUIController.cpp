#include "PlayerHealthUIController.h"

#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// 開始処理
void PlayerHealthUIController::Start()
{
	// シーンからCanvasMediatorを取得
	_canvasMediator = GetActor()->GetScene()->GetActorManager().FindByClass<CanvasMediator>(ActorTag::UI);
	// ユーザーが操作するプレイヤーのHPUIの場合は、CanvasMediatorに登録
	if (_isUserControlled && _canvasMediator.lock())
	{
		_canvasMediator.lock()->SetUserHealthUI(this);
	}
	// ユーザーが操作しないプレイヤーのHPUIの場合は、CanvasMediatorに登録
	else if (!_isUserControlled && _canvasMediator.lock())
	{
		_canvasMediator.lock()->AddOtherUserHealthUI(this);
	}
	// 画像読み込み
	LoadTexture(FrameSprite, L"Data/Texture/UI/Frame.png", Sprite::CenterAlignment::LeftCenter);
	SetPosition(FrameSprite, InitialPosition); // 初期位置を設定
	LoadTexture(MaskSprite, L"Data/Texture/UI/Mask.png", Sprite::CenterAlignment::LeftCenter);
	SetPosition(MaskSprite, InitialPosition); // 初期位置を設定
	LoadTexture(GaugeSprite, L"Data/Texture/UI/HPGauge.png", Sprite::CenterAlignment::LeftCenter);
	SetPosition(GaugeSprite, InitialPosition); // 初期位置を設定
	LoadTexture(DamageGaugeSprite, L"Data/Texture/UI/DamageGauge.png", Sprite::CenterAlignment::LeftCenter);
	SetPosition(DamageGaugeSprite, InitialPosition); // 初期位置を設定
}
// 削除処理
void PlayerHealthUIController::Deleted()
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
	// HPに応じてスケールを変更
	if (_damageable.lock())
	{
		float healthRatio = _damageable.lock()->GetHealth() / _damageable.lock()->GetMaxHealth();
		healthRatio = std::clamp(healthRatio, 0.0f, 1.0f); // 0.0fから1.0fの範囲に制限
		SetScale(GaugeSprite, Vector2(healthRatio, 1.0f)); // HPに応じて横幅を変更

		// ダメージゲージのスケールを更新
		float damageGaugeScaleX = EasingLerp(
			GetScale(DamageGaugeSprite).x,
			GetScale(GaugeSprite).x,
			_damageGaugeScaleSpeed * elapsedTime);
		SetScale(DamageGaugeSprite, Vector2(damageGaugeScaleX, InitialPosition.y)); // ダメージゲージの横幅を変更
	}
}
// GUI描画
void PlayerHealthUIController::DrawGui()
{
	ImGui::Text(u8"キャンバス: %s", _canvasMediator.lock() ? u8"有効" : u8"無効");
	ImGui::DragFloat(u8"ダメージゲージスケール速度", &_damageGaugeScaleSpeed, 0.1f, 0.0f, 10.0f);
	UIController::DrawGui();
}

void PlayerHealthUIController::DrawUI(const RenderContext& rc, const Vector2& offset, const Vector2& offsetScale)
{
	const RenderState* renderState = rc.renderState;
	// フレーム部分の描画
	rc.deviceContext->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 1);
	SpriteRender(FrameSprite, rc, offset, offsetScale);

	// マスク部分の描画
	rc.deviceContext->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::SpriteMask), 1);
	SpriteRender(MaskSprite, rc, offset, offsetScale);

	// ゲージ部分の描画
	rc.deviceContext->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::SpriteApplyMask), 0);
	SpriteRender(DamageGaugeSprite, rc, offset, offsetScale);
	SpriteRender(GaugeSprite, rc, offset, offsetScale);
	
	rc.deviceContext->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 1);
}
