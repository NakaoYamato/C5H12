#pragma once

#include <unordered_map>
#include <string>
#include <memory>
template<class T>
class StateBase
{
public:
	StateBase(T* owner) :_owner(owner) {}
	virtual ~StateBase() {}

	// 名前の取得
	virtual const char* GetName() const = 0;

	// 全て継承先で実装させる必要があるため純粋仮想関数で実装
	// ステートに入った時のメソッド
	virtual void OnEnter() = 0;
	// ステートで実行するメソッド
	virtual void OnExecute(float elapsedTime) = 0;
	// ステートから出ていくときのメソッド
	virtual void OnExit() = 0;
	// GUi描画
	virtual void DrawGui() {};

	// 親ステート設定
	virtual void SetParentState(StateBase* parentState)
	{
		_parentState = parentState;
	}
protected:
	T* _owner;
	StateBase* _parentState = nullptr;
};

template<typename T>
class HierarchicalStateBase : public StateBase<T>
{
public:
	HierarchicalStateBase(T* owner) : StateBase<T>(owner) {}
	virtual ~HierarchicalStateBase() {}

	// 開始処理
	virtual void Enter()
	{
		OnEnter();
	}
	// 更新処理
	virtual void Execute(float elapsedTime)
	{
		if (_subState)
			_subState->OnExecute(elapsedTime);
		OnExecute(elapsedTime);
	}
	// 終了処理
	virtual void Exit()
	{
		if (_subState)
			_subState->OnExit();
		OnExit();
	}

#pragma region 仮想関数
	// 名前の取得
	virtual const char* GetName() const = 0;
	// GUi描画
	virtual void DrawGui() {};
	// サブステート設定
	virtual void SetSubState(std::string key)
	{
		// 2層目ステートセット
		_subState = _stateMap.at(key).get();
		_subState->OnEnter();
	}
	// サブステート変更
	virtual void ChangeSubState(std::string key)
	{
		// 2層目のステート切り替え
		if (_subState != nullptr)
			_subState->OnExit();
		SetSubState(key);
	}
	// サブステート登録
	virtual void RegisterSubState(std::shared_ptr<StateBase<T>> state)
	{
		state->SetParentState(this);
		_stateMap.insert(std::make_pair(state->GetName(), state));
	}
	// サブステートの名前番号取得
	virtual const char* GetSubStateName()
	{
		if (_subState == nullptr)
			return "";
		return _subState->GetName();
	}
#pragma endregion

protected:
#pragma region 仮想関数
	// ステートに入った時のメソッド
	virtual void OnEnter() = 0;
	// ステートで実行するメソッド
	virtual void OnExecute(float elapsedTime) {};
	// ステートから出ていくときのメソッド
	virtual void OnExit() {};
#pragma endregion

protected:
	using StateMap = std::unordered_map<std::string, std::shared_ptr<StateBase<T>>>;
	StateMap _stateMap;
	StateBase<T>* _subState = nullptr;
};