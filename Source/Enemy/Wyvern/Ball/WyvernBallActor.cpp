#include "WyvernBallActor.h"

void WyvernBallActor::OnCreate()
{
	auto sphere = this->AddCollider<SphereCollider>();
	auto effectController = this->AddComponent<EffectController>();
	effectController->LoadEffekseerEffect(0, "./Data/Effect/Effekseer/Wyvern/FireBallMain.efk");
	effectController->LoadEffekseerEffect(1, "./Data/Effect/Effekseer/Wyvern/FireBallFinish.efk");

	effectController->GetEffectData(0)->SetScale(Vector3(0.5f, 0.5f, 0.5f));
	effectController->GetEffectData(1)->SetScale(Vector3(0.5f, 0.5f, 0.5f));
	_ballController = this->AddComponent<WyvernBallController>();
	sphere->SetLayer(CollisionLayer::Attack);
}
