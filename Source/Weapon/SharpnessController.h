#pragma once

#include "../../Library/Component/Component.h"
#include "../../Source/Common/DamageSender.h"
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

	// Á”ï
	void ConsumeSharpness(float amount);
	// ‰ñ•œ
	void RecoverSharpness(float amount);

	// Ø‚ê–¡”{—¦æ“¾
	float GetSharpnessRate();
	// a‚ê–¡ƒQ[ƒWİ’è
	void SetSharpnessGauge(const std::vector<float>& gauge)
	{
		_baseSharpnessGauge = gauge;
		_currentSharpnessGauge = gauge;
	}
	// •Ší‚Ì‚à‚Æ‚à‚Æ‚Ìa‚ê–¡ƒQ[ƒWæ“¾
	const std::vector<float>& GetBaseSharpnessGauge() const { return _baseSharpnessGauge; }
	// Œ»İ‚Ìa‚ê–¡ƒQ[ƒWæ“¾
	const std::vector<float>& GetCurrentSharpnessGauge() const { return _currentSharpnessGauge; }
private:
	static float SharpnessRateTable[WeaponSharpnessLevelMax];

	std::weak_ptr<DamageSender> _damageSender;

	// •Ší‚Ì‚à‚Æ‚à‚Æ‚Ìa‚ê–¡ƒQ[ƒW
	std::vector<float> _baseSharpnessGauge;
	// Œ»İ‚Ìa‚ê–¡ƒQ[ƒW
	std::vector<float> _currentSharpnessGauge;
};