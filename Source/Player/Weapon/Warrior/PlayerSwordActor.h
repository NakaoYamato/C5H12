#pragma once

#include "../WeaponActor.h"

class PlayerSwordActor : public WeaponActor
{
public:
	~PlayerSwordActor()override {}

	// ¶¬ˆ—
	void OnCreate() override;
};