#pragma once

#include <functional>

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

/// <summary>
/// コールバック関数でビヘイビア実行判定を行うクラス
/// </summary>
/// <typeparam name="T"></typeparam>
template<class T>
class BehaviorCallbackJudgment : public BehaviorJudgmentBase<T>
{
public:
	BehaviorCallbackJudgment(T* owner, std::function<bool()> judgment) :
		BehaviorJudgmentBase<T>(owner),
		_judgmentFunc(judgment)
	{}
	// 判定処理
	virtual bool Judgment() override
	{
		return _judgmentFunc();
	}
private:
	std::function<bool()> _judgmentFunc;
};