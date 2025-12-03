#pragma once

#include "../../Library/Actor/Actor.h"

class FenceActor : public Actor
{
public:
	FenceActor() = default;
	~FenceActor() override {}

	// ¶¬ˆ—
	void OnCreate() override;
};
