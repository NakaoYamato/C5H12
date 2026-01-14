#pragma once

#include "../../Library/Actor/UI/Menu/MenuUIActor.h"

class QuestBoardUIActor : public MenuUIActor
{
public:
	QuestBoardUIActor() = default;
	~QuestBoardUIActor() override {}
	// ¶¬ˆ—
	void OnCreate() override;
};