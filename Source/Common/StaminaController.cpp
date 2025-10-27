#include "StaminaController.h"

#include <imgui.h>

// 更新処理
void StaminaController::Update(float elapsedTime)
{
	if (_isStaminaRecover)
	{
		RecoverStamina(_staminaRecoverSpeed * elapsedTime);
	}
}

// Gui描画
void StaminaController::DrawGui()
{
	ImGui::DragFloat(u8"現在のスタミナ", &_stamina, 1.0f, 0.0f, _maxStamina);
	ImGui::DragFloat(u8"最大スタミナ", &_maxStamina, 1.0f, 0.0f, 1000.0f);
	ImGui::DragFloat(u8"スタミナ自然回復速度", &_staminaRecoverSpeed, 0.1f, 0.0f, 100.0f);
	ImGui::Checkbox(u8"スタミナが自動回復するか", &_isStaminaRecover);
}

// スタミナ初期化
void StaminaController::ResetStamina(float maxStamina)
{
	_maxStamina = maxStamina;
	_stamina = maxStamina;
}

// スタミナを消費する
// falseでは消費失敗、trueで消費成功
bool StaminaController::ConsumeStamina(float amount, bool forcedExecution)
{
	if (_stamina >= amount || forcedExecution)
	{
		_stamina = std::clamp(_stamina - amount, 0.0f, _maxStamina);
		return true;
	}
	return false;
}

// スタミナを回復する
void StaminaController::RecoverStamina(float amount)
{
	_stamina = std::clamp(_stamina + amount, 0.0f, _maxStamina);
}
