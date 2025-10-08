#pragma once

#include "StateBase.h"

template<typename T>
class StateMachineBase
{
public:
	using HierarchicalStateMap = std::unordered_map<std::string, std::shared_ptr<HierarchicalStateBase<T>>>;
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
		_currentState->Execute(elapsedTime);
	}
	// ステート変更
	virtual void ChangeState(std::string key)
	{
		if (_currentState != nullptr)
		{
			_currentState->Exit();
			_previousStateName = _currentState->GetName();
		}

#ifdef _DEBUG
		if (_stateMap.find(key) == _stateMap.end())
			return;
#endif // _DEBUG
		_currentState = _stateMap[key].get();
		_currentState->Enter();
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
	// 2層目ステート変更
	virtual void ChangeSubState(std::string key)
	{
		_currentState->ChangeSubState(key);
	}
	// 2層目ステート登録
	virtual void RegisterSubState(std::string key, std::shared_ptr<StateBase<T>> subState)
	{
		_stateMap.at(key)->RegisterSubState(subState);
	}
	// ステート取得
	HierarchicalStateBase<T>* GetState() { return _currentState; }
	// ステートマップ取得
	HierarchicalStateMap& GetStateMap() { return _stateMap; }
	// 前のステート名取得
	std::string GetPreviousStateName() const { return _previousStateName; }
protected:
	// ステートのマップ
	HierarchicalStateMap _stateMap;

	// 現在のステート
	HierarchicalStateBase<T>* _currentState = nullptr;
	// 前のステート名
	std::string _previousStateName = "";
};
