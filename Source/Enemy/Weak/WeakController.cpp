#include "WeakController.h"

#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// 開始処理
void WeakController::Start()
{
	_enemyController = GetActor()->GetComponent<EnemyController>();
}
// 更新処理
void WeakController::Update(float elapsedTime)
{
}
// GUI描画
void WeakController::DrawGui()
{
	ImGui::Text(u8"ジョグ移動速度 : %.1f", _jogSpeed);
	ImGui::DragFloat(u8"ジョグ移動速度", &_jogSpeed, 0.1f, 0.0f, 10.0f, "%.1f m/s");
	ImGui::Text(u8"ダッシュ移動速度 : %.1f", _dashSpeed);
	ImGui::DragFloat(u8"ダッシュ移動速度", &_dashSpeed, 0.1f, 0.0f, 10.0f, "%.1f m/s");
}
