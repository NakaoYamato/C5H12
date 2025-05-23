#pragma once

/// <summary>
/// 行動結果
/// </summary>
enum class BehaviorActionState
{
	Run,		// 実行中
	Failed,		// 実行失敗
	Complete,	// 実行成功
};

/// <summary>
/// ビヘイビア行動処理基底クラス
/// </summary>
/// <typeparam name="T"></typeparam>
template<class T>
class BehaviorActionBase
{
public:
	BehaviorActionBase(T* owner): _owner(owner) {}
	// 開始処理(仮想関数)
	virtual void OnEnter() {}
	// 実行処理(純粋仮想関数)
	virtual BehaviorActionState Run(float elapsedTime) = 0;
	// 終了処理(仮想関数)
	virtual void OnExit() {};
protected:
	T*	_owner;
	int _step = 0;
};