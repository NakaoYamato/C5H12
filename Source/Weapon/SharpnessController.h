#pragma once

#include "../../Library/Component/Component.h"
#include "WeaponData.h"

class SharpnessController : public Component
{
public:
	SharpnessController() = default;
	~SharpnessController() override = default;

	// –¼‘Oæ“¾
	const char* GetName() const override { return "SharpnessController"; }
	// ŠJnˆ—
	void Start() override;
	// XVˆ—
	void Update(float elapsedTime) override;
	// Gui•`‰æ
	void DrawGui() override;

	// Ø‚ê–¡”{—¦æ“¾
	float GetSharpnessRate();
	// a‚ê–¡ƒQ[ƒWİ’è
	void SetSharpnessGauge(const std::vector<float>& gauge)
	{
		_baseSharpnessGauge = gauge;
		_currentSharpnessGauge = gauge;
	}
private:
	static float SharpnessRateTable[WeaponSharpnessLevelMax];

	// •Ší‚Ì‚à‚Æ‚à‚Æ‚Ìa‚ê–¡ƒQ[ƒW
	std::vector<float> _baseSharpnessGauge;
	// Œ»İ‚Ìa‚ê–¡ƒQ[ƒW
	std::vector<float> _currentSharpnessGauge;
};