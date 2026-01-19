#pragma once

#include "../EnemyActor.h"
#include "../../Library/Actor/ActorFactory.h"
#include "../../Library/Component/ShapeController.h"

class DummyActor : public EnemyActor
{
public:
    ~DummyActor()override {}
    // ê∂ê¨éûèàóù
    void OnCreate() override;
};

// ActorFactoryÇ…ìoò^
_REGISTER_ACTOR_FACTORY(DummyActor, "DummyActor")
