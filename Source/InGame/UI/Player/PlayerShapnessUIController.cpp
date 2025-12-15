#include "PlayerShapnessUIController.h"

#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// 開始処理
void PlayerShapnessUIController::Start()
{
	_spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();
}

// 更新処理
void PlayerShapnessUIController::Update(float elapsedTime)
{
	auto spriteRenderer = _spriteRenderer.lock();
	auto sharpnessController = _sharpnessController.lock();
	if (!spriteRenderer || !sharpnessController)
		return;
	
	auto& baseGauge = sharpnessController->GetBaseSharpnessGauge();
	auto& currentGauge = sharpnessController->GetCurrentSharpnessGauge();

	// 色設定
	Vector4 color = WeaponData::GetSharpnessColor(static_cast<int>(currentGauge.size()));
	spriteRenderer->SetColor(GaugeSpr, color);

	// 各ゲージの総量取得
	float baseGaugeValue{}, currentGaugeValue{};
	for (auto value : baseGauge)
		baseGaugeValue += value;
	for (auto value : currentGauge)
		currentGaugeValue += value;
	if (baseGaugeValue <= 0.0f)
		return;
	// バーの大きさ設定
	Vector2 scale{};
	scale.x = _barScaleYMax * (1.0f - (currentGaugeValue / baseGaugeValue));
	scale.y = 5.0f;
	spriteRenderer->GetRectTransform(BarSpr).SetLocalScale(scale);
}

// GUI描画
void PlayerShapnessUIController::DrawGui()
{
}
