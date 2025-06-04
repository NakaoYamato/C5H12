#include "WyvernBreathActor.h"

#include "WyvernBreathController.h"

void WyvernBreathActor::OnCreate()
{
	auto capsule = this->AddCollider<CapsuleCollider>();
	_breathController = this->AddComponent<WyvernBreathController>(capsule);

	capsule->SetLayer(CollisionLayer::Attack);
}
