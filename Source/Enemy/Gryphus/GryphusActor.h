#pragma once

#include "../EnemyActor.h"

class GryphusActor : public EnemyActor
{
public:
    static const char* GetModelFilePath()
    {
        return "./Data/Model/Enemy/Gryphus/Lion Gryphus_A1.fbx";
    }

public:
    GryphusActor() {}
    ~GryphusActor() override {}
    // ¶¬ˆ—
    void OnCreate() override;
};