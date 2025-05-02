#include "PlayerController.h"

#include <imgui.h>

void PlayerController::Start()
{
	_rigidbody = GetActor()->GetComponent<Rigidbody>();
	_playerInput = GetActor()->GetComponent<PlayerInput>();
}

void PlayerController::Update(float elapsedTime)
{
	auto playerInput = _playerInput.lock();
	if (playerInput)
	{
		Vector3 movement = playerInput->GetMovement();
		if (movement.x != 0.0f || movement.z != 0.0f)
		{
			_rigidbody.lock()->AddForce(movement * _moveSpeed * elapsedTime);
		}
	}
}

// GUI•`‰æ
void PlayerController::DrawGui()
{
	ImGui::DragFloat("MoveSpeed", &_moveSpeed, 0.01f, 0.0f, 100.0f);
}
