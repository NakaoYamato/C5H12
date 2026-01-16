#pragma once

#include "Component.h"
#include "../../Library/Algorithm/StateMachine/StateMachine.h"

class StateMachine
{
public:
	StateMachine() {}
	~StateMachine() {}
	// 開始処理
	virtual void Start() {}
	// 実行処理
	virtual void Execute(float elapsedTime) {}
	// GUI描画
	virtual void DrawGui() {}
	// ステート変更
	virtual void ChangeState(const char* mainStateName, const char* subStateName) = 0;
	// ステート名取得
	virtual const char* GetStateName() = 0;
	// サブステート名取得
	virtual const char* GetSubStateName() = 0;
	// セットアップ済みかどうか
	bool IsSetup() const { return _isSetup; }
	// セットアップ済みフラグを設定
	void SetSetup(bool isSetup) { _isSetup = isSetup; }
	// ステートの変更を受け付けるか取得
	bool CanChangeState() const { return _canChangeState; }
	// ステートの変更を受け付けるか設定
	void SetCanChangeState(bool canChangeState) { _canChangeState = canChangeState; }
private:
	// セットアップ済みフラグ
	bool _isSetup = false;
	// ステートの変更を受け付けるか
	bool _canChangeState = true;
};

class StateController : public Component
{
public:
	StateController(std::shared_ptr<StateMachine> stateMachine);
	~StateController() override {}
	// 名前取得
	const char* GetName() const override { return "StateController"; }
	// 開始処理
	void Start() override;
	// 遅延更新処理
	void LateUpdate(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;
	// ステート変更
	void ChangeState(const char* mainStateName, const char* subStateName);
	// ステート名取得
	const char* GetStateName() { return _stateMachine->GetStateName(); }
	// サブステート名取得
	const char* GetSubStateName() { return _stateMachine->GetSubStateName(); }
	// ステートマシン取得
	std::shared_ptr<StateMachine> GetStateMachine() { return _stateMachine; }
private:
	std::shared_ptr<StateMachine> _stateMachine = nullptr;
};

template<typename T>
class StateController2 : public Component
{
public:
	using SM = StateMachineBase<T>;

public:
	StateController2(std::shared_ptr<SM> stateMachine) :_stateMachine(stateMachine) {}
	~StateController2() override {}
	// 名前取得
	const char* GetName() const override { return "StateController2"; }
	// 開始処理
	void Start() final override
	{
		OnStart();
		// セットアップ済みフラグを立てる
		SetSetup(true);
	}
	// 遅延更新処理
	void LateUpdate(float elapsedTime) final override
	{
		OnLateUpdate(elapsedTime);
		_stateMachine->Update(elapsedTime);
	}
	// GUI描画
	void DrawGui() final override
	{
		OnDrawGui();
	}

	// ステート変更
	virtual void ChangeState(const char* mainStateName, const char* subStateName)
	{
		// ステートの変更を受け付けない場合は何もしない
		if (!CanChangeState())
			return;

		// 遷移先が無効な場合は何もしない
		if (mainStateName == nullptr || mainStateName[0] == '\0')
			return;

		std::string currentMainState = GetStateName();
		std::string currentSubState = GetSubStateName();

		// 現在のステートと変更先が違うなら変更
		if (currentMainState != mainStateName)
			_stateMachine->ChangeState(mainStateName);
		// サブステートがあるなら変更
		if (subStateName != nullptr && subStateName[0] != '\0')
		{
			// 現在のサブステートと変更先が違うなら変更
			if (currentSubState != subStateName)
				_stateMachine->ChangeSubState(subStateName);
		}
	}
	// ステート名取得
	virtual const char* GetStateName()
	{
		if (!_stateMachine->GetState())
			return "";
		return _stateMachine->GetState()->GetName();
	}
	// サブステート名取得
	virtual const char* GetSubStateName()
	{
		if (!_stateMachine->GetState())
			return"";
		return _stateMachine->GetState()->GetSubStateName();
	}
	// ステートマシン取得
	std::shared_ptr<SM> GetStateMachine() { return _stateMachine; }

	// セットアップ済みかどうか
	bool IsSetup() const { return _isSetup; }
	// セットアップ済みフラグを設定
	void SetSetup(bool isSetup) { _isSetup = isSetup; }
	// ステートの変更を受け付けるか取得
	bool CanChangeState() const { return _canChangeState; }
	// ステートの変更を受け付けるか設定
	void SetCanChangeState(bool canChangeState) { _canChangeState = canChangeState; }
protected:
	virtual void OnStart() {}
	virtual void OnLateUpdate(float elapsedTime) {}
	virtual void OnDrawGui() {}

protected:
	std::shared_ptr<SM> _stateMachine = nullptr;

	// セットアップ済みフラグ
	bool _isSetup = false;
	// ステートの変更を受け付けるか
	bool _canChangeState = true;
};
