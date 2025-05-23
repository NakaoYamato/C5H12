#pragma once

#include <vector>
#include <string>
#include <memory>
#include "BehaviorActionBase.h"
#include "BehaviorJudgmentBase.h"
#include "BehaviorData.h"

// 選択ルール
enum class SelectRule
{
	Non,				// ルールなし
	Priority,			// 優先順位
	Sequence,			// シーケンス
	SequentialLooping,	// シーケンシャルルーピング
	Random,				// ランダム
};

template<class T>
class BehaviorNodeBase : public std::enable_shared_from_this<BehaviorNodeBase<T>>
{
public:
	using NodeTempPtr		= std::shared_ptr<BehaviorNodeBase<T>>;
	using JudgmentTempPtr	= std::shared_ptr<BehaviorJudgmentBase<T>>;
	using ActionTempPtr		= std::shared_ptr<BehaviorActionBase<T>>;
public:
	// コンストラクタ
	BehaviorNodeBase(
		std::string name, 
		NodeTempPtr parent,
		NodeTempPtr sibling,
		int priority,
		SelectRule selectRule,
		JudgmentTempPtr judgment, 
		ActionTempPtr action,
		int hierarchyNo) :
		_name(name),
		_parent(parent),
		_sibling(sibling),
		_priority(priority),
		_selectRule(selectRule),
		_judgment(judgment), 
		_action(action), 
		_hierarchyNo(hierarchyNo),
		_children(NULL)
	{
	}
	// デストラクタ
	~BehaviorNodeBase() {}
#pragma region アクセサ
	// 名前ゲッター
	std::string GetName() { return _name; }
	// 親ノードゲッター
	NodeTempPtr GetParent() { return _parent; }
	// 子ノードゲッター
	NodeTempPtr GetChild(int index)
	{
		if (_children.size() <= index)
			return nullptr;
		return _children.at(index);
	}
	// 子ノードゲッター(末尾)
	NodeTempPtr GetLastChild()
	{
		if (_children.size() == 0)
			return nullptr;
		return _children.at(_children.size() - 1);
	}
	// 子ノードゲッター(先頭)
	NodeTempPtr GetTopChild()
	{
		if (_children.size() == 0)
			return nullptr;
		return _children.at(0);
	}
	// 兄弟ノードゲッター
	NodeTempPtr GetSibling() { return _sibling; }
	// 階層番号ゲッター
	int GetHirerchyNo() { return _hierarchyNo; }
	// 優先順位ゲッター
	int GetPriority() { return _priority; }
	// 親ノードセッター
	void SetParent(NodeTempPtr parent) { this->_parent = parent; }
	// 子ノード追加
	void AddChild(NodeTempPtr child) { _children.push_back(child); }
	// 兄弟ノードセッター
	void SetSibling(NodeTempPtr sibling) { this->_sibling = sibling; }
	// 行動データを持っているか
	bool HasAction() { return _action != nullptr ? true : false; }
	// ノード全削除
	void ClearChildren()
	{
		for (auto& child : _children)
		{
			child->ClearChildren();
		}
		_children.clear();
	}
#pragma endregion

	// 実行可否判定
	bool Judgment()
	{
		if (_judgment != nullptr)
			return _judgment->Judgment();
		return true;
	}
	// 優先順位選択
	NodeTempPtr SelectPriority(std::vector<NodeTempPtr>* list)
	{
		NodeTempPtr selectNode = nullptr;
		unsigned int priority = INT_MAX;

		// 番優先順位が高いノードを探してselectNodeに格納
		for (int i = 0; i < list->size(); i++)
		{
			// list->at(i)->priority が低いほど優先
			if (list->at(i)->_priority < priority)
			{
				priority = list->at(i)->_priority;
				selectNode = list->at(i);
			}
		}

		return selectNode;
	}
	// ランダム選択
	NodeTempPtr SelectRandom(std::vector<NodeTempPtr>* list)
	{
		int selectNo = 0;
		int listSize = static_cast<int>(list->size());
		if (listSize <= 0)
			return nullptr;

		selectNo = std::rand() % listSize;

		// listのselectNo番目の実態をリターン
		return (*list).at(selectNo);
	}
	// シーケンス選択
	NodeTempPtr SelectSequence(std::vector<NodeTempPtr>* list, BehaviorData<T>* data)
	{
		int step = 0;

		// 指定されている中間ノードのがシーケンスがどこまで実行されたか取得する
		step = data->GetSequenceStep(_name);

		// 中間ノードに登録されているノード数以上の場合、
		if (step >= _children.size())
		{
			switch (_selectRule)
			{
			case SelectRule::Non:
				break;
			case SelectRule::Priority:
				break;
			case SelectRule::Sequence:
				return nullptr;
				break;
			case SelectRule::SequentialLooping:
				step = 0;
				break;
			case SelectRule::Random:
				break;
			default:
				break;
			}
		}
		// 実行可能リストに登録されているデータの数だけループを行う
		for (auto itr = list->begin(); itr != list->end(); itr++)
		{
			// 子ノードが実行可能リストに含まれているか
			if (_children.at(step)->GetName() == (*itr)->GetName())
			{
				data->PushSequenceNode(_children.at(step)->GetParent().get());
				data->SetSequenceStep(_children.at(step)->GetParent()->GetName(), step + 1);
				return _children.at(step);
			}
		}
		// 指定された中間ノードに実行可能ノードがないのでnullptrをリターンする
		return nullptr;
	}
	// ノード検索
	NodeTempPtr SearchNode(std::string searchName)
	{
		// 名前が一致
		if (_name == searchName)
		{
			// テンプレートでshard_from_thisを使用するために
			// this->shared_from_this()を使用する
			return this->shared_from_this();
		}
		else 
		{
			// 子ノードで検索
			for (auto itr = _children.begin(); itr != _children.end(); itr++)
			{
				NodeTempPtr ret = (*itr)->SearchNode(searchName);

				if (ret != nullptr)
				{
					return ret;
				}
			}
		}

		return nullptr;
	}
	// ノード推論
	NodeTempPtr Inference(BehaviorData<T>* data)
	{
		std::vector<NodeTempPtr> list;
		NodeTempPtr result = nullptr;

		// childrenの数だけループを行う。
		for (int i = 0; i < _children.size(); i++)
		{
			// children.at(i)->judgmentがnullptrでなければ
			if (_children.at(i)->_judgment != nullptr)
			{
				// tureであればlistにchildren.at(i)を追加していく
				if (_children.at(i)->_judgment->Judgment())
					list.push_back(_children.at(i));
			}
			else 
			{
				// 判定クラスがなければ無条件に追加
				list.push_back(_children.at(i));
			}
		}

		// 選択ルールでノード決め
		switch (_selectRule)
		{
			// 優先順位
		case SelectRule::Priority:
			result = SelectPriority(&list);
			break;
			// ランダム
		case SelectRule::Random:
			result = SelectRandom(&list);
			break;
			// シーケンス
		case SelectRule::Sequence:
		case SelectRule::SequentialLooping:
			result = SelectSequence(&list, data);
			break;
		}

		if (result != nullptr)
		{
			// 行動があれば終了
			if (result->HasAction() == true)
			{
				return result;
			}
			else 
			{
				// 決まったノードで推論開始
				result = result->Inference(data);
			}
		}

		return result;
	}
	// 開始
	void Enter()
	{
		if (_action != nullptr)
			_action->OnEnter();
	}
	// 実行
	BehaviorActionState Run(float elapsedTime)
	{
		if (_action != nullptr)
			return _action->Run(elapsedTime);
		return BehaviorActionState::Failed;
	}
	// 終了
	void Exit()
	{
		if (_action != nullptr)
			_action->OnExit();
	}
protected:
	std::string		_name;			// 名前
	SelectRule		_selectRule;	// 選択ルール
	JudgmentTempPtr	_judgment;		// 判定クラス
	ActionTempPtr	_action;		// 実行クラス
	unsigned int	_priority;		// 優先順位
	NodeTempPtr		_parent;		// 親ノード
	NodeTempPtr		_sibling;		// 兄弟ノード
	int				_hierarchyNo;	// 階層番号
	std::vector<NodeTempPtr>		_children;		// 子ノード
};