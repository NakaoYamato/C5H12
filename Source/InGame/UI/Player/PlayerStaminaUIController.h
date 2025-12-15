#pragma once

#include "../../Library/Actor/UI/UIActor.h"
#include "../../Library/Component/SpriteRenderer.h"
#include "../../Source/Common/StaminaController.h"

class PlayerStaminaUIController : public Component
{
public:
	PlayerStaminaUIController() = default;
    ~PlayerStaminaUIController() override {}
    // 名前取得
    const char* GetName() const override { return "PlayerStaminaUIController"; }
    // 開始処理
    void Start() override;
    // 更新処理
    void Update(float elapsedTime) override;
    // GUI描画
    void DrawGui() override;

	void SetStaminaController(std::shared_ptr<StaminaController> staminaController) { _staminaController = staminaController; }

private:
    std::weak_ptr<SpriteRenderer> _spriteRenderer;
    std::weak_ptr<StaminaController> _staminaController;
    // 各種スプライトの名前
    const std::string FrameSpr = "Frame";
    const std::string MaskSpr = "Mask";
    const std::string GaugeSpr = "Gauge";
    const std::string GaugeEndSprite = "GaugeEnd";

    // マスクの開始地点
    float _maskStartPosX = 44.0f;
    // マスクの開始スケール
    float _maskStartScaleX = 25.37f;

    // ゲージ先端の開始地点
    float _gaugeEndStartX = 440.0f;
    // ゲージ先端の終了地点
    float _gaugeEndEndX = 44.0f;
};