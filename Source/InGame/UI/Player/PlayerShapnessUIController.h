#pragma once

#include "../../Library/Actor/UI/UIActor.h"
#include "../../Library/Component/SpriteRenderer.h"
#include "../../Source/Weapon/SharpnessController.h"

class PlayerShapnessUIController : public Component
{
public:
	PlayerShapnessUIController() = default;
	~PlayerShapnessUIController() override = default;
	// 名前取得
	const char* GetName() const override { return "PlayerShapnessUIController"; }

	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

	void SetSharpnessController(const std::shared_ptr<SharpnessController>& sharpnessController)
	{
		_sharpnessController = sharpnessController;
	}

private:
	std::weak_ptr<SpriteRenderer> _spriteRenderer;
	std::weak_ptr<SharpnessController> _sharpnessController;

	const std::string GaugeSpr = "Gauge";
	const std::string BarSpr = "Bar";

	float _barScaleYMax = 16.45f;
};