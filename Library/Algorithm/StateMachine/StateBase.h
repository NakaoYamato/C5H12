#pragma once

#include <unordered_map>
#include <string>
#include <memory>
template<class T>
class StateBase
{
public:
	StateBase(T* owner) :owner(owner) {}
	virtual ~StateBase() {}

	// 名前の取得
	virtual const char* GetName() const = 0;

	// 全て継承先で実装させる必要があるため純粋仮想関数で実装
	// ステートに入った時のメソッド
	virtual void Enter() = 0;
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime) = 0;
	// ステートから出ていくときのメソッド
	virtual void Exit() = 0;
	// GUi描画
	virtual void DrawGui() {};
protected:
	T* owner;
};

template<typename T>
class HierarchicalStateBase : public StateBase<T>
{
public:
	HierarchicalStateBase(T* owner) : StateBase<T>(owner) {}
	virtual ~HierarchicalStateBase() {}

	// 名前の取得
	virtual const char* GetName() const = 0;
	// 全て継承先で実装させる必要があるため純粋仮想関数で実装
	// ステートに入った時のメソッド
	virtual void Enter() = 0;
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)
	{
		if (subState)
			subState->Execute(elapsedTime);
	}
	// ステートから出ていくときのメソッド
	virtual void Exit()
	{
		if (subState)
			subState->Exit();
	}
	// GUi描画
	virtual void DrawGui() {};
	// サブステート設定
	virtual void SetSubState(std::string key)
	{
		// 2層目ステートセット
		subState = stateMap.at(key).get();
		subState->Enter();
	}
	// サブステート変更
	virtual void ChangeSubState(std::string key)
	{
		// 2層目のステート切り替え
		if (subState != nullptr)
			subState->Exit();
		SetSubState(key);
	}
	// サブステート登録
	virtual void RegisterSubState(std::shared_ptr<StateBase<T>> state)
	{
		stateMap.insert(std::make_pair(state->GetName(), state));
	}
	// サブステートの名前番号取得
	virtual std::string GetSubStateName()
	{
		if (subState == nullptr)
			return "";
		return subState->GetName();
	}

protected:
	using StateMap = std::unordered_map<std::string, std::shared_ptr<StateBase<T>>>;
	StateMap stateMap;
	StateBase<T>* subState = nullptr;
};