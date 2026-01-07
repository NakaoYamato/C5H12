#pragma once

#include "../../Library/Actor/Actor.h"
#include "../../Library/Actor/ActorFactory.h"

class QuestBoardActor : public Actor
{
public:
	QuestBoardActor() = default;
	~QuestBoardActor() override {}

	// ê∂ê¨éûèàóù
	void OnCreate() override;
};

// ActorFactoryÇ…ìoò^
_REGISTER_ACTOR_FACTORY(QuestBoardActor, "QuestBoardActor")
