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
	virtual void OnEnter() = 0;
	// ステートで実行するメソッド
	virtual void OnExecute(float elapsedTime) = 0;
	// ステートから出ていくときのメソッド
	virtual void OnExit() = 0;
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

	/// <summary>
	/// 開始処理
	/// </summary>
	void Enter()
	{
		OnEnter();
	}
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="elapsedTime"></param>
	void Execute(float elapsedTime)
	{
		if (subState)
			subState->OnExecute(elapsedTime);
		OnExecute(elapsedTime);
	}
	/// <summary>
	/// 終了処理
	/// </summary>
	void Exit()
	{
		if (subState)
			subState->OnExit();
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
		subState = stateMap.at(key).get();
		subState->OnEnter();
	}
	// サブステート変更
	virtual void ChangeSubState(std::string key)
	{
		// 2層目のステート切り替え
		if (subState != nullptr)
			subState->OnExit();
		SetSubState(key);
	}
	// サブステート登録
	virtual void RegisterSubState(std::shared_ptr<StateBase<T>> state)
	{
		stateMap.insert(std::make_pair(state->GetName(), state));
	}
	// サブステートの名前番号取得
	virtual const char* GetSubStateName()
	{
		if (subState == nullptr)
			return "";
		return subState->GetName();
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
	StateMap stateMap;
	StateBase<T>* subState = nullptr;
};