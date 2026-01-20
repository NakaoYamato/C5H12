#pragma once

#include "../../Library/Actor/Actor.h"

class GameManager : public Actor
{
public:
	GameManager() = default;
	~GameManager() override {}

	// ¶¬ˆ—
	void OnCreate() override;
};