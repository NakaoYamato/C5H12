#pragma once

#include "../../Source/InGame/CanvasMediator.h"
#include "../../Source/Common/Damageable.h"

class PlayerHealthUIController : public UIController
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
	// 削除処理
	void OnDelete() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

	void DrawUI(const RenderContext& rc)override;
private:
	// ユーザーが操作するプレイヤーか
	const bool _isUserControlled = true;
	std::weak_ptr<CanvasMediator> _canvasMediator;
	std::weak_ptr<Damageable> _damageable;

	// 各種スプライトの名前
	const std::string FrameSprite	= "Frame";
	const std::string MaskSprite	= "Mask";
	const std::string GaugeSprite	= "Gauge";
	const std::string DamageGaugeSprite	= "DamageGauge";

	// 初期位置
	const Vector2 InitialPosition = Vector2(50.0f, 40.0f);

	// ダメージゲージのスケール速度
	float _damageGaugeScaleSpeed = 5.0f;
};