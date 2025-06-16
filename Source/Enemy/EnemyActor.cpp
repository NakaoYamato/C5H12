#include "EnemyActor.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/Common/Targetable.h"

void EnemyActor::OnCreate()
{
	_charactorController = this->AddComponent<CharactorController>();
	_damageable = this->AddComponent<Damageable>();
	auto targetable = this->AddComponent<Targetable>();

	targetable->SetFaction(Targetable::Faction::Enemy);
}
