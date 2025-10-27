#pragma once

#include "../../Library/Component/Component.h"

class StaminaController : public Component
{
public:
	StaminaController() {}
	virtual ~StaminaController() override = default;
	// 名前取得
	const char* GetName() const override { return "StaminaController"; }
	// 更新処理
	void Update(float elapsedTime) override;
	// Gui描画
	void DrawGui() override;
	// スタミナ初期化
	virtual void ResetStamina(float maxStamina);
	// スタミナを消費する
	// falseでは消費失敗、trueで消費成功
	// forcedExecutionがtrueの場合、スタミナが足りなくても強制的に消費する
	virtual bool ConsumeStamina(float amount, bool forcedExecution = false);
	// スタミナを回復する
	virtual void RecoverStamina(float amount);

#pragma region アクセサ
	// スタミナ取得
	float GetStamina() const { return _stamina; }
	// 最大スタミナ取得
	float GetMaxStamina() const { return _maxStamina; }
	// スタミナ自然回復速度取得
	float GetStaminaRecoverSpeed() const { return _staminaRecoverSpeed; }
	// スタミナが自然回復するか取得
	bool IsStaminaRecover() const { return _isStaminaRecover; }

	// スタミナ自然回復速度設定
	void SetStaminaRecoverSpeed(float speed) { _staminaRecoverSpeed = speed; }
	// スタミナが自然回復するか設定
	void SetIsStaminaRecover(bool isRecover) { _isStaminaRecover = isRecover; }
#pragma endregion
private:
	// スタミナ
	float _stamina = 100.0f;
	// 最大スタミナ
	float _maxStamina = 100.0f;
	// スタミナ自然回復速度
	float _staminaRecoverSpeed = 5.0f;
	// スタミナが自然回復するか
	bool _isStaminaRecover = true;
};