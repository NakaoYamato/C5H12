#pragma once

#include "../EnemyActor.h"
#include "../../Library/Component/ShapeController.h"

class DummyActor : public EnemyActor
{
public:
    ~DummyActor()override {}
    // ¶¬ˆ—
    void OnCreate() override;
};