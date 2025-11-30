#pragma once

#include "../../Source/InGame/InGameCanvasActor.h"
#include "../../Source/Common/Damageable.h"

class PlayerHealthUIController : public Component
{
public:
	PlayerHealthUIController(bool isUserControlled, std::shared_ptr<Damageable> damageable) :
		_isUserControlled(isUserControlled),
		_damageable(damageable) {}
	~PlayerHealthUIController() override {}
	// 名前取得
	const char* GetName() const override { return "PlayerHPUIController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;
private:
	// ユーザーが操作するプレイヤーか
	const bool _isUserControlled = true;
	std::weak_ptr<Damageable> _damageable;
	std::weak_ptr<SpriteRenderer> _spriteRenderer;

	// 各種スプライトの名前
	const std::string FrameSprite	= "Frame";
	const std::string MaskSprite	= "Mask";
	const std::string GaugeSprite	= "Gauge";
	const std::string DamageGaugeMaskSprite	= "DamageGaugeMask";
	const std::string GaugeEndSprite	= "GaugeEnd";
	const std::string DamageGaugeSprite	= "DamageGauge";

	// ダメージゲージのスケール速度
	float _damageGaugeScaleSpeed = 5.0f;

	// マスクの開始地点
	float _maskStartPosX = 44.0f;
	// マスクの開始スケール
	float _maskStartScaleX = 25.37f;

	// ゲージ先端の開始地点
    float _gaugeEndStartX = 440.0f;
	// ゲージ先端の終了地点
    float _gaugeEndEndX = 44.0f;
};