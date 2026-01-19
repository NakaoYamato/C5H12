#include "StateController.h"

StateController::StateController(std::shared_ptr<StateMachine> stateMachine)
{
	_stateMachine = stateMachine;
}
// 開始処理
void StateController::Start()
{
	_stateMachine->Start();
	// セットアップ済みフラグを立てる
	_stateMachine->SetSetup(true);
}
// 遅延更新処理
void StateController::LateUpdate(float elapsedTime)
{
	_stateMachine->Execute(elapsedTime);
}
// GUI描画
void StateController::DrawGui()
{
	_stateMachine->DrawGui();
}
// ステート変更
void StateController::ChangeState(const char* mainStateName, const char* subStateName)
{
	_stateMachine->ChangeState(mainStateName, subStateName);
}
