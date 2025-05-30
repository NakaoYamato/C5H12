#include "EnemyActor.h"

#include "../../Library/Scene/Scene.h"

void EnemyActor::OnCreate()
{
	_charactorController = this->AddComponent<CharactorController>();
	_damageable = this->AddComponent<Damageable>();
}
