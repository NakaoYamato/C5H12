#pragma once

#include "../WeaponActor.h"

class PlayerSwordActor : public WeaponActor
{
public:
	static const char* GetModelFilePath()
	{
		return "./Data/Model/Player/Sword/SM_Sword_1.fbx";
	}
public:
	~PlayerSwordActor()override {}

	// ¶¬ˆ—
	void OnCreate() override;
};