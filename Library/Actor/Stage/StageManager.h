#pragma once

#include "../../Library/Actor/Actor.h"

class StageManager : public Actor
{
public:
	StageManager() = default;
	~StageManager() override = default;
	// ¶¬ˆ—
	void OnCreate() override;
};
