#pragma once

#include "../../Library/Actor/Actor.h"

#include "../../Library/Component/CharactorController.h"
#include "../../Source/Common/Damageable.h"
#include "../../Source/Common/Targetable.h"

class EnemyActor : public Actor
{
public:
	EnemyActor() {}
	~EnemyActor() override {}
	// ¶¬ˆ—
	void OnCreate() override;
protected:
	std::weak_ptr<CharactorController> _charactorController;
	std::weak_ptr<Damageable> _damageable;
	std::weak_ptr<Targetable> _targetable;
};