#pragma once

#include "../EnemyActor.h"

class WeakActor : public EnemyActor
{
public:
	static const char* GetModelFilePath()
	{
		return "./Data/Model/Enemy/Raptors/Import001.fbx";
	}
public:
	WeakActor() {}
	~WeakActor() override {}
	// ¶¬ˆ—
	void OnCreate() override;
};