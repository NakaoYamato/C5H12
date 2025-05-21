#include "WyvernActionDerived.h"

#include "../../Source/BehaviorTree/Enemy/Wyvern/WyvernBehaviorTree.h"

BehaviorActionState WyvernNormalAction::Run(float elapsedTime)
{
	return BehaviorActionState::Run;
}

BehaviorActionState WyvernIdleAction::Run(float elapsedTime)
{
	return BehaviorActionState::Run;
}
