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
		stateMap.clear();
	}
	// 更新処理
	virtual void Update(float elapsedTime)
	{
		currentState->Execute(elapsedTime);
	}
	// ステート変更
	virtual void ChangeState(std::string key)
	{
		if (currentState != nullptr)
			currentState->Exit();

#ifdef _DEBUG
		if (stateMap.find(key) == stateMap.end())
			return;
#endif // _DEBUG
		currentState = stateMap[key].get();
		currentState->Enter();
	}
	// ステート登録
	virtual void RegisterState(std::shared_ptr<HierarchicalStateBase<T>> state)
	{
		// 親ステート登録
		stateMap.insert(std::make_pair(state->GetName(), state));
	}
	// 現在のステートの名前取得
	virtual std::string GetStateName()
	{
		if (currentState == nullptr)
			return "";
		return currentState->GetName();
	}
	// 2層目ステート変更
	virtual void ChangeSubState(std::string key)
	{
		currentState->ChangeSubState(key);
	}
	// 2層目ステート登録
	virtual void RegisterSubState(std::string key, std::shared_ptr<StateBase<T>> subState)
	{
		stateMap.at(key)->RegisterSubState(subState);
	}
	// ステート取得
	HierarchicalStateBase<T>* GetState() { return currentState; }

	HierarchicalStateMap& GetStateMap() { return stateMap; }
protected:
	// ステートのマップ
	HierarchicalStateMap stateMap;

	// 現在のステート
	HierarchicalStateBase<T>* currentState = nullptr;
};
