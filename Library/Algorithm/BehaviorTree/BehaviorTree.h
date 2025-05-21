#pragma once

#include <string>

#include "BehaviorNodeBase.h"

template<class T>
class BehaviorTree
{
public:
	BehaviorTree(T* owner) : _owner(owner) {}
	~BehaviorTree() 
	{
		NodeAllClear(_root.get());
	}

	// 実行ノードを推論する
	BehaviorNodeBase<T>* ActiveNodeInference(BehaviorData<T>* data)
	{
		// データをリセットして開始
		data->Init();
		return _root->Inference(data);
	}

	// シーケンスノードから推論開始
	BehaviorNodeBase<T>* SequenceBack(BehaviorNodeBase<T>* sequenceNode, BehaviorData<T>* data)
	{
		return sequenceNode->Inference(data);
	}

	// ノード追加
	void AddNode(
		std::string parentName,
		std::string entryName,
		int priority,
		SelectRule selectRule,
		std::shared_ptr<BehaviorJudgmentBase<T>> judgment,
		std::shared_ptr<BehaviorActionBase<T>> action)
	{
		if (parentName != "")
		{
			std::shared_ptr<BehaviorNodeBase<T>> parentNode = _root->SearchNode(parentName);

			if (parentNode != nullptr)
			{
				std::shared_ptr<BehaviorNodeBase<T>> sibling = parentNode->GetLastChild();
				std::shared_ptr<BehaviorNodeBase<T>> addNode = std::make_shared<BehaviorNodeBase<T>>(entryName, parentNode, sibling, priority, selectRule, judgment, action, parentNode->GetHirerchyNo() + 1);

				parentNode->AddChild(addNode);
			}
		}
		else 
		{
			if (_root == nullptr)
			{
				_root = std::make_shared<BehaviorNodeBase<T>>(entryName, nullptr, nullptr, priority, selectRule, judgment, action, 1);
			}
		}
	}

	// 実行
	BehaviorNodeBase<T>* Run(BehaviorNodeBase<T>* actionNode, BehaviorData<T>* data, float elapsedTime)
	{
		// ノード実行
		BehaviorActionState state = actionNode->Run(elapsedTime);

		// 正常終了
		if (state == BehaviorActionState::Complete)
		{
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
			// 失敗は終了
		}
		else if (state == BehaviorActionState::Failed) {
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

