#pragma once

#include "../../Library/Actor/Actor.h"
#include "../../Library/Component/ShapeController.h"

class DummyActor : public Actor
{
public:
    ~DummyActor()override {}
    // ¶¬ˆ—
    void OnCreate() override;
};