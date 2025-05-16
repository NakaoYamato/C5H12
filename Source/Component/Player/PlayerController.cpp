#include "PlayerController.h"

#include "../../External/nameof/include/nameof.hpp"
#include "../../External/magic_enum/include/magic_enum/magic_enum.hpp"
#include <imgui.h>

void PlayerController::Start()
{
	_charactorController = GetActor()->GetComponent<CharactorController>();
	_playerInput = GetActor()->GetComponent<PlayerInput>();
	_animator = GetActor()->GetComponent<Animator>();

	_charactorController.lock()->SetMaxSpeedXZ(5.0f);

    _stateMachine = std::make_unique<PlayerStateMachine>(this, _animator.lock().get());
}

void PlayerController::Update(float elapsedTime)
{
	if (_isUserControlled)
	{
		auto playerInput = _playerInput.lock();
		if (playerInput)
		{
			Vector3 movement = playerInput->GetMovement();
			if (movement.x != 0.0f || movement.z != 0.0f)
			{
				_charactorController.lock()->AddForce(movement * _moveSpeed);
				_charactorController.lock()->SetRotateToDirection(true);
			}
			else
			{
				_charactorController.lock()->AddForce(Vector3::Normalize(_charactorController.lock()->GetVelocityXZ()) * -_friction);
				_charactorController.lock()->SetRotateToDirection(false);
			}

			_isAttack = false;
			if (playerInput->GetInputFlag() & PlayerInput::Inputs::Attack)
			{
                _state = PlayerState::Attack1;
				_isAttack = true;
			}
		}
	}

	// 行動処理
    _stateMachine->Execute(elapsedTime);
}

// GUI描画
void PlayerController::DrawGui()
{
    // プレイヤーの状態を表示
    ImGui::Text(u8"プレイヤーの状態 : %s", nameof::nameof_enum(_state).data());
    ImGui::Separator();
	ImGui::DragFloat(u8"移動速度", &_moveSpeed, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat(u8"摩擦", &_friction, 0.01f, 0.0f, 100.0f);

	ImGui::Separator();
	_stateMachine->DrawGui();
}
