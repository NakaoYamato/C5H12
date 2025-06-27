#include "BehaviorController.h"

#include <imgui.h>

BehaviorController::BehaviorController(std::shared_ptr<BehaviorTree> behaviorTree)
{
	_behaviorTree = behaviorTree;
}

void BehaviorController::Start()
{
	_behaviorTree->Start();
}

void BehaviorController::Update(float elapsedTime)
{
	if (_isExecute && _behaviorTree)
	{
		_behaviorTree->Execute(elapsedTime);
	}
}

void BehaviorController::DrawGui()
{
	if (_behaviorTree)
	{
		bool flag = _isExecute;
		ImGui::Checkbox(u8"ビヘイビアツリーを実行する", &flag);
		_behaviorTree->DrawGui();
	}
}
