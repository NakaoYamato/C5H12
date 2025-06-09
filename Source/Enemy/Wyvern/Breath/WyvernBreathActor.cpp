#include "WyvernBreathActor.h"

#include "WyvernBreathController.h"

void WyvernBreathActor::OnCreate()
{
	auto capsule = this->AddCollider<CapsuleCollider>();
	auto particleController = this->AddComponent<ParticleController>("./Data/Particle/Wyvern/Breath.json");
	_breathController = this->AddComponent<WyvernBreathController>();

	capsule->SetLayer(CollisionLayer::Attack);
}
