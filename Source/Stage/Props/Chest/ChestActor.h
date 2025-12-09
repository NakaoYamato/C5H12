#pragma once

#include "../../Library/Actor/Actor.h"
#include "../../Library/Actor/ActorFactory.h"

class ChestActor : public Actor
{
public:
	ChestActor() = default;
	~ChestActor() override {}

	// ê∂ê¨éûèàóù
	void OnCreate() override;
};

// ActorFactoryÇ…ìoò^
_REGISTER_ACTOR_FACTORY(ChestActor, "ChestActor")
