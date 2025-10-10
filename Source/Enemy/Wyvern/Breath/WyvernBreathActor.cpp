#include "WyvernBreathActor.h"

#include "WyvernBreathController.h"

void WyvernBreathActor::OnCreate()
{
	auto capsule0 = this->AddCollider<CapsuleCollider>();
	auto capsule1 = this->AddCollider<CapsuleCollider>();
	auto capsule2 = this->AddCollider<CapsuleCollider>();
	auto effectController = this->AddComponent<EffectController>();
	effectController->LoadParticleEffect(0, "./Data/Particle/Wyvern/Breath.json");
	_breathController = this->AddComponent<WyvernBreathController>();

	capsule0->SetLayer(CollisionLayer::Attack);
	capsule1->SetLayer(CollisionLayer::Attack);
	capsule2->SetLayer(CollisionLayer::Attack);
}
