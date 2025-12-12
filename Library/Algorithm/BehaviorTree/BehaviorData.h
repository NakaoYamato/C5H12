#pragma once

#include <vector>
#include <stack>
#include <map>

// 前方宣言
template<class T>
class BehaviorNodeBase;

template<class T>
class BehaviorData
{
public:
	// コンストラクタ
	BehaviorData() 
	{
		Init();
	}
	// シーケンスノードのプッシュ
	void PushSequenceNode(BehaviorNodeBase<T>* node) { _sequenceStack.push(node); }
	// シーケンスノードのポップ
	BehaviorNodeBase<T>* PopSequenceNode()
	{
		// 空ならNULL
		if (_sequenceStack.empty() != 0)
		{
			return nullptr;
		}
		BehaviorNodeBase<T>* node = _sequenceStack.top();
		if (node != nullptr)
		{
			// 取り出したデータを削除
			_sequenceStack.pop();
		}
		return node;
	}
	// シーケンスステップのゲッター
	int GetSequenceStep(std::string name)
	{
		if (_runSequenceStepMap.count(name) == 0)
		{
			_runSequenceStepMap.insert(std::make_pair(name, 0));
		}

		return _runSequenceStepMap.at(name);
	}
	// シーケンスステップのセッター
	void SetSequenceStep(std::string name, int step)
	{
		_runSequenceStepMap.at(name) = step;
	}
	// 初期化
	void Init()
	{
		_runSequenceStepMap.clear();
		while (_sequenceStack.size() > 0)
		{
			_sequenceStack.pop();
		}
	}

private:
	// 実行する中間ノードをスタック
	std::stack<BehaviorNodeBase<T>*>	_sequenceStack;
	// 実行中の中間ノードのステップを記録
	std::map<std::string, int>			_runSequenceStepMap;
};
