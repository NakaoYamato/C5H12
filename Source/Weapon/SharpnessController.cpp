#include "SharpnessController.h"

#include <imgui.h>

float SharpnessController::SharpnessRateTable[WeaponSharpnessLevelMax] = {
	0.50f, // 赤
	0.75f, // 橙
	1.00f, // 黄
	1.05f, // 緑
	1.20f, // 青
	1.32f, // 白
	1.39f, // 紫
};

// 開始処理
void SharpnessController::Start()
{
}

// 更新処理
void SharpnessController::Update(float elapsedTime)
{
}

// Gui描画
void SharpnessController::DrawGui()
{
	// 切れ味ゲージGUI描画
	ImGui::Text(u8"現在の切れ味ゲージ");
	WeaponData::DrawSharpnessGaugeGui(_currentSharpnessGauge, false);
}

// 切れ味倍率取得
float SharpnessController::GetSharpnessRate()
{
	return 0.0f;
}
