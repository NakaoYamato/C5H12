#pragma once

/// <summary>
/// ビヘイビア実行判定基底クラス
/// </summary>
/// <typeparam name="T"></typeparam>
template<class T>
class BehaviorJudgmentBase
{
public:
	BehaviorJudgmentBase(T* owner) :_owner(owner) {}
	// 判定処理(純粋仮想関数)
	virtual bool Judgment() = 0;
protected:
	T* _owner;
};