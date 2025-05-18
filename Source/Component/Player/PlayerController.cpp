#include "PlayerController.h"

#include "../../Library/DebugSupporter/DebugSupporter.h"
#include "../../External/nameof/include/nameof.hpp"
#include "../../External/magic_enum/include/magic_enum/magic_enum.hpp"
#include <imgui.h>

void PlayerController::Start()
{
	_charactorController = GetActor()->GetComponent<CharactorController>();
	_animator = GetActor()->GetComponent<Animator>();

	_charactorController.lock()->SetMaxSpeedXZ(5.0f);

    _stateMachine = std::make_unique<PlayerStateMachine>(this, _animator.lock().get());
}

void PlayerController::Update(float elapsedTime)
{
	// 行動処理
    _stateMachine->Execute(elapsedTime);

	// 受けたダメージを初期化
	_sustainedDamage = 0;
}

// GUI描画
void PlayerController::DrawGui()
{
    // プレイヤーの状態を表示
    ImGui::Text(u8"プレイヤーの状態 : %s", nameof::nameof_enum(_state).data());
    ImGui::Separator();
	ImGui::DragFloat(u8"移動速度", &_moveSpeed, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat(u8"摩擦", &_friction, 0.01f, 0.0f, 100.0f);

	if (ImGui::Button(u8"ダメージを受ける"))
	{
		_sustainedDamage = 1;
	}
	if (ImGui::Button(u8"大ダメージを受ける"))
	{
		_sustainedDamage = _knockbackDamage;
	}
	if (ImGui::Button(u8"死亡"))
	{
		_isDead = true;
	}

	ImGui::Separator();
	_stateMachine->DrawGui();
}
