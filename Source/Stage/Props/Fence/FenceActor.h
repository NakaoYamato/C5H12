#pragma once

#include "../../Library/Actor/Actor.h"
#include "../../Library/Actor/ActorFactory.h"

class FenceActor : public Actor
{
public:
	FenceActor() = default;
	~FenceActor() override {}

	// ê∂ê¨éûèàóù
	void OnCreate() override;
};

// ActorFactoryÇ…ìoò^
_REGISTER_ACTOR_FACTORY(FenceActor, "FenceActor")
