#include "PlayerController.h"

#include <imgui.h>

void PlayerController::Start()
{
	_charactorController = GetActor()->GetComponent<CharactorController>();
	_playerInput = GetActor()->GetComponent<PlayerInput>();
	_animator = GetActor()->GetComponent<Animator>();

	_charactorController.lock()->SetMaxSpeedXZ(5.0f);

	_animator.lock()->PlayAnimation(u8"Idle1", true);
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
		}
	}
}

// GUI•`‰æ
void PlayerController::DrawGui()
{
	ImGui::DragFloat(u8"ˆÚ“®‘¬“x", &_moveSpeed, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat(u8"–€ŽC", &_friction, 0.01f, 0.0f, 100.0f);
}
