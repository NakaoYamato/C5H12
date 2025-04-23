#include "PlayerController.h"

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
			_rigidbody.lock()->AddForce(movement);
		}
	}
}
