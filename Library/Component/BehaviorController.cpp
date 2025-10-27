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
#ifdef _DEBUG
		if (_isDebugExecute)
			_behaviorTree->Execute(elapsedTime);
#else
		_behaviorTree->Execute(elapsedTime);
#endif // _DEBUG
	}

#ifdef USE_IMGUI
	// GUI表示
	if (_behaviorTree && _isShowGui)
	{
		if (ImGui::Begin(u8"ビヘイビアツリー"))
		{
			_behaviorTree->DrawGui();
		}
		ImGui::End();
	}
#endif
}

void BehaviorController::DrawGui()
{
	if (_behaviorTree)
	{
		bool flag = _isExecute;
		ImGui::Checkbox(u8"ビヘイビアツリーを実行する", &flag);
		ImGui::Checkbox(u8"デバッグ用実行フラグ", &_isDebugExecute);
		ImGui::Text(u8"現在の実行ノード: %s", _behaviorTree->GetActiveNodeName().c_str());
		ImGui::Checkbox(u8"ビヘイビアツリーのGUIを表示する", &_isShowGui);
	}
}
