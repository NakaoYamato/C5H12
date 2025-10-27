#pragma once

#include "../../Library/Algorithm/BehaviorTree/BehaviorActionBase.h"

// 前方宣言
class WyvernBehaviorTree;

#pragma region 指定のステートが完了するまで実行し続ける
class WyvernCompleteStateAction : public BehaviorActionBase<WyvernBehaviorTree>
{
public:
	WyvernCompleteStateAction(
		WyvernBehaviorTree* owner,
		const char* stateName,
		float requiredStamina = 0.0f) :
		BehaviorActionBase(owner),
		_stateName(stateName),
		_requiredStamina(requiredStamina)
	{}
	// 開始処理
	void Enter()override;
	// 実行処理
	BehaviorActionState Execute(float elapsedTime) override;
private:
	// 実行するステート名
	const char* _stateName = nullptr;
	// 実行するためのスタミナ量
	float _requiredStamina = 0.0f;
};
#pragma endregion

#pragma region 指定のサブステートが完了するまで実行し続ける
class WyvernCompleteSubStateAction : public BehaviorActionBase<WyvernBehaviorTree>
{
public:
	WyvernCompleteSubStateAction(
		WyvernBehaviorTree* owner,
		const char* stateName,
		const char* subStateName,
		float requiredStamina = 0.0f) :
		BehaviorActionBase(owner),
		_stateName(stateName),
		_subStateName(subStateName),
		_requiredStamina(requiredStamina) {
	}
	// 開始処理
	void Enter()override;
	// 実行処理
	BehaviorActionState Execute(float elapsedTime) override;
private:
	// 実行するステート名
	const char* _stateName = nullptr;
	// 実行するサブステート名
	const char* _subStateName = nullptr;
	// 実行するためのスタミナ量
	float _requiredStamina = 0.0f;
};
#pragma endregion

#pragma region 咆哮
class WyvernRoarAction : public WyvernCompleteStateAction
{
public:
	WyvernRoarAction(WyvernBehaviorTree* owner,
		const char* startStateName) :
		WyvernCompleteStateAction(owner, startStateName)
	{
	}
	// 終了処理
	void Exit() override;
private:
	bool _hasRoared = false;
};
#pragma endregion

#pragma region 指定時間再生するアクション
class WyvernTimerAction : public BehaviorActionBase<WyvernBehaviorTree>
{
public:
	WyvernTimerAction(
		WyvernBehaviorTree* owner,
		const char* startStateName,
		float time) :
		BehaviorActionBase(owner),
		_startStateName(startStateName),
		_time(time) {
	}
	// 開始処理
	void Enter()override;
	// 実行処理
	BehaviorActionState Execute(float elapsedTime) override;
private:
	const char* _startStateName = nullptr; // 実行するステート名
	float _time = 0.0f;
	float _timer = 0.0f;
};
#pragma endregion
