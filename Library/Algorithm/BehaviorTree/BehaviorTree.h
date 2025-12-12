#pragma once

#include <string>

#include "BehaviorNodeBase.h"

template<class T>
class BehaviorTreeBase
{
public:
	BehaviorTreeBase(T* owner) : _owner(owner) {}
	~BehaviorTreeBase() 
	{
		NodeAllClear(_root.get());
	}

	// 実行ノードを推論する
	BehaviorNodeBase<T>* ActiveNodeInference(BehaviorData<T>* data)
	{
		// データをリセットして開始
		data->Init();
		BehaviorNodeBase<T>* node = _root->Inference(data).get();
		if (node != nullptr)
		{
			// ノードの開始処理
			node->Enter();
		}
		return node;
	}

	// シーケンスノードから推論開始
	BehaviorNodeBase<T>* SequenceBack(BehaviorNodeBase<T>* sequenceNode, BehaviorData<T>* data)
	{
		BehaviorNodeBase<T>* node = sequenceNode->Inference(data).get();
		if (node != nullptr)
		{
			// ノードの開始処理
			node->Enter();
		}
		return node;
	}

	// ルートノード取得
	std::shared_ptr<BehaviorNodeBase<T>> GetRoot()
	{
		if (_root == nullptr)
			_root = std::make_shared<BehaviorNodeBase<T>>("Root", nullptr, nullptr, 0, SelectRule::Priority, nullptr, nullptr, 1);
		return _root;
	}

	// 指定したノードの取得
	BehaviorNodeBase<T>* GetNode(std::string searchName)
	{
		if (_root == nullptr)
			return nullptr;
		return _root->SearchNode(searchName).get();
	}

	// 実行
	BehaviorNodeBase<T>* Execute(BehaviorNodeBase<T>* actionNode, BehaviorData<T>* data, float elapsedTime)
	{
		// ノード実行
		BehaviorActionState state = actionNode->Execute(elapsedTime);

		// 正常終了
		if (state == BehaviorActionState::Complete)
		{
			// 終了処理
			actionNode->Exit();

			// シーケンスの途中かを判断
			BehaviorNodeBase<T>* sequenceNode = data->PopSequenceNode();

			// 途中じゃないなら終了
			if (sequenceNode == nullptr)
			{
				return nullptr;
			}
			else 
			{
				// 途中ならそこから始める
				return SequenceBack(sequenceNode, data);
			}
		}
		// 失敗は終了
		else if (state == BehaviorActionState::Failed) 
		{
			// 終了処理
			actionNode->Exit();

			return nullptr;
		}

		// 現状維持
		return actionNode;
	}

private:
	// ノード全削除
	void NodeAllClear(BehaviorNodeBase<T>* delNode)
	{
		delNode->ClearChildren();
	}

private:
	T* _owner = nullptr;
	// ルートノード
	std::shared_ptr<BehaviorNodeBase<T>> _root;
};

