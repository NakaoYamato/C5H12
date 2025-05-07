#pragma once

#include "../../Library/Actor/Actor.h"

class PlayerActor : public Actor
{
public:
	~PlayerActor()override {}

	// ¶¬ˆ—
	void OnCreate() override;

	// íœˆ—
	void Destroy() override;
private:
    std::weak_ptr<Actor> _swordActor;
    std::weak_ptr<Actor> _shieldActor;
};