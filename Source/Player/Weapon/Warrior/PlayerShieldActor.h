#pragma once

#include "../WeaponActor.h"

class PlayerShieldActor : public WeaponActor
{
public:
	static const char* GetModelFilePath()
	{
		return "./Data/Model/Player/Shield/SM_Round_Metal_Shield.fbx";
	}
public:
	~PlayerShieldActor()override {}

	// ¶¬ˆ—
	void OnCreate() override;
};