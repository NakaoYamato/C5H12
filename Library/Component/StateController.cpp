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

class TestSMC;

class TestHSB : public HierarchicalStateBase<TestSMC>
{
public:
	TestHSB(TestSMC* stateMachine) : HierarchicalStateBase<TestSMC>(stateMachine) {}
	~TestHSB() override {}
	const char* GetName() const { return "TestHSB"; }
	// 開始処理
	virtual void OnEnter() override
	{
	}
	// 更新処理
	virtual void OnExecute(float elapsedTime) override
	{
	}
	// 終了処理
	virtual void OnExit() override
	{
	}
};

class TestSMC : public StateController2<TestSMC>
{
public:
	TestSMC(std::shared_ptr<StateController2::SM> stateMachine) :
		StateController2(stateMachine)
	{
	}

	void OnStart() override
	{
		_stateMachine->RegisterState(std::make_shared<TestHSB>(this));
	}
	void OnLateUpdate(float elapsedTime) override
	{

	}
};

static TestSMC testSMCInstance(std::make_shared<StateMachineBase<TestSMC>>());