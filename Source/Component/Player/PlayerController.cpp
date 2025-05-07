#include "PlayerController.h"

#include <imgui.h>

void PlayerController::Start()
{
	_charactorController = GetActor()->GetComponent<CharactorController>();
	_playerInput = GetActor()->GetComponent<PlayerInput>();

	_charactorController.lock()->SetMaxSpeedXZ(5.0f);

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
			}
			else
			{
				_charactorController.lock()->AddForce(Vec3Normalize(_charactorController.lock()->GetVelocity()) * -_friction);
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
