#pragma once

#include <stack>

#include "StateBase.h"

template<typename T>
class StateMachineBase
{
public:
	using HierarchicalStateMap = std::unordered_map<std::string, std::shared_ptr<HierarchicalStateBase<T>>>;
	using StatePairName = std::pair<std::string, std::string>;
public:
	// コンストラクタ
	StateMachineBase() {}
	// デストラクタ
	virtual ~StateMachineBase()
	{
		_stateMap.clear();
	}
	// 更新処理
	virtual void Update(float elapsedTime)
	{
		if (_currentState != nullptr)
			_currentState->Execute(elapsedTime);
	}
	// ステート変更
	virtual void ChangeState(std::string key)
	{
		if (_currentState != nullptr)
		{
			_currentState->Exit();
			_previousStateName = _currentState->GetName();
			_previousSubStateName = _currentState->GetSubStateName();
		}

#ifdef _DEBUG
		if (_stateMap.find(key) == _stateMap.end())
			return;
#endif // _DEBUG
		_currentState = _stateMap[key].get();
		_currentState->Enter();
	}
	// ステートの終了
	virtual void EndState()
	{
		if (_currentState != nullptr)
		{
			_currentState->Exit();
			_previousStateName = _currentState->GetName();
			_previousSubStateName = _currentState->GetSubStateName();
			_currentState = nullptr;
		}
	}
	// ステート登録
	virtual void RegisterState(std::shared_ptr<HierarchicalStateBase<T>> state)
	{
		// 親ステート登録
		_stateMap.insert(std::make_pair(state->GetName(), state));
	}
	// 現在のステートの名前取得
	virtual std::string GetStateName()
	{
		if (_currentState == nullptr)
			return "";
		return _currentState->GetName();
	}

#pragma region サブステート
	// サブステート変更
	virtual void ChangeSubState(std::string key)
	{
		_currentState->ChangeSubState(key);
	}
	// サブステート登録
	virtual void RegisterSubState(std::string key, std::shared_ptr<StateBase<T>> subState)
	{
		_stateMap.at(key)->RegisterSubState(subState);
	}
	// サブステート名前取得
	virtual std::string GetSubStateName()
	{
		if (_currentState == nullptr)
			return "";
		return _currentState->GetSubStateName();
	}
#pragma endregion

#pragma region スタックステート
	// スタックしているステートに変更
	// スタックが空ならステートを終了
	virtual void ChangeStateStack()
	{
		// スタックが空ならステートを終了
		if (_stateStack.empty())
		{
			EndState();
			return;
		}
		auto& stateNames = _stateStack.top();
		_stateStack.pop();
		ChangeState(stateNames.first);
		if (!stateNames.second.empty())
			ChangeSubState(stateNames.second);
	}
	// スタックにステートを追加
	virtual void PushStateStack(const std::string& stateName, const std::string& subStateName = "")
	{
		_stateStack.push(std::make_pair(stateName, subStateName));
	}
	// スタックをクリア
	virtual void ClearStateStack()
	{
		while (!_stateStack.empty())
			_stateStack.pop();
	}
#pragma endregion

#pragma region アクセサ
	// ステート取得
	HierarchicalStateBase<T>* GetState() { return _currentState; }
	// ステートマップ取得
	HierarchicalStateMap& GetStateMap() { return _stateMap; }
	// 前のステート名取得
	const std::string& GetPreviousStateName() const { return _previousStateName; }
	// 前のサブステート名取得
	const std::string& GetPreviousSubStateName() const { return _previousSubStateName; }
	// スタックステート取得
	std::stack<StatePairName>& GetStateStack() { return _stateStack; }
#pragma endregion

protected:
	// ステートのマップ
	HierarchicalStateMap _stateMap;

	// 現在のステート
	HierarchicalStateBase<T>* _currentState = nullptr;
	// 前のステート名
	std::string _previousStateName = "";
    // 前のサブステート名
    std::string _previousSubStateName = "";

	// スタックステート
	std::stack<StatePairName> _stateStack;
};
