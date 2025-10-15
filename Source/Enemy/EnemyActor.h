#pragma once

#include "../../Library/Actor/Actor.h"

#include "../../Library/Component/CharactorController.h"
#include "../../Source/Common/Damageable.h"
#include "../../Source/Common/Targetable.h"
#include "../../Source/Common/CombatStatusController.h"

class EnemyActor : public Actor
{
public:
	EnemyActor() {}
	~EnemyActor() override {}
	// ê∂ê¨éûèàóù
	void OnCreate() override;
protected:
	std::weak_ptr<CharactorController> _charactorController;
	std::weak_ptr<Damageable> _damageable;
	std::weak_ptr<Targetable> _targetable;
	std::weak_ptr<CombatStatusController> _combatStatus;
};