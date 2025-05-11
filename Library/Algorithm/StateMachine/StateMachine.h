#pragma once

#include "StateBase.h"

template<typename T>
class StateMachine
{
public:
	using HierarchicalStateMap = std::unordered_map<std::string, std::shared_ptr<HierarchicalStateBase<T>>>;
public:
	// コンストラクタ
	StateMachine() {}
	// デストラクタ
	virtual ~StateMachine()
	{
		stateMap.clear();
	}
	// 更新処理
	virtual void Update(float elapsedTime)
	{
		currentState->Execute(elapsedTime);
	}
	// ステートセット
	virtual void SetState(std::string key)
	{
#ifdef _DEBUG
		if (stateMap.find(key) == stateMap.end())
			return;
#endif // _DEBUG
		currentState = stateMap.at(key).get();
		currentState->Enter();
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
