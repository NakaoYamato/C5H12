#pragma once

#include "../WeaponActor.h"

class PlayerGreatSwordActor : public WeaponActor
{
public:
	static const char* GetModelFilePath()
	{
		return "./Data/Model/Player/GreatSword/Weapon/Marcus' sword1.fbx";
	}
public:
	~PlayerGreatSwordActor()override {}

	// ¶¬ˆ—
	void OnCreate() override;
};