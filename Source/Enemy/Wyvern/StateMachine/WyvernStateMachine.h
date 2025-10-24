#pragma once

#include <variant>
#include "../../Library/Component/StateController.h"
#include "../../Library/Math/Vector.h"
#include "../../Source/Common/Damageable.h"
#include "../../Source/Common/CombatStatusController.h"
#include "../../Source/Common/RoarController.h"
#include "../../Source/Common/DamageSender.h"

// 前方宣言
class EnemyController;
class WyvernController;
class Animator;

class WyvernStateMachine : public StateMachine
{
public:
	WyvernStateMachine(Actor* owner);
	~WyvernStateMachine() {}
	// 開始処理
	void Start() override;
	// 実行処理
	void Execute(float elapsedTime) override;
	// Gui描画
	void DrawGui() override;

#pragma region アクセサ
	// ステート変更
	void ChangeState(const char* mainStateName, const char* subStateName) override;
	// ステート名取得
	const char* GetStateName() override;
	// サブステート名取得
	const char* GetSubStateName() override;

	StateMachineBase<WyvernStateMachine>& GetBase() { return _stateMachine; }
	EnemyController* GetEnemy() { return _enemy; }
	WyvernController* GetWyvern() { return _wyvern; }
	Animator* GetAnimator() { return _animator; }
	DamageSender* GetDamageSender() { return _damageSender; }
	CombatStatusController* GetCombatStatus() { return _combatStatus; }
	Damageable* GetDamageable() { return _damageable; }

	// ブレス攻撃のグローバル位置を設定
	const Vector3& GetBreathGlobalPosition() const { return _breathGlobalPosition; }
	// 火球攻撃のグローバル位置を設定	
	const Vector3& GetFireBallGlobalPosition() const { return _fireBallGlobalPosition; }
	// キャンセルイベントを取得
	bool CallCancelEvent() const { return _callCancelEvent; }
	// ブレス攻撃イベントを取得
	bool CallFireBreathEvent() const { return _callFireBreath; }
	// 火球攻撃イベントを取得	
	bool CallFireBallEvent() const { return _callFireBall; }
	// ターゲットを見つめるイベントを取得
	bool CallLookAtTargetEvent() const { return _callLookAtTarget; }
#pragma endregion

private:
	StateMachineBase<WyvernStateMachine> _stateMachine;
	EnemyController* _enemy = nullptr;
	WyvernController* _wyvern = nullptr;
	Animator* _animator = nullptr;
	Damageable* _damageable = nullptr;
	DamageSender* _damageSender = nullptr;
	CombatStatusController* _combatStatus = nullptr;
	RoarController* _roarController = nullptr;

	// ブレス攻撃のグローバル位置
	Vector3 _breathGlobalPosition = Vector3::Zero;
	// 火球攻撃のグローバル位置
	Vector3 _fireBallGlobalPosition = Vector3::Zero;
	// 頭の回転時のターゲットオフセット
	Vector3 _headRotationOffset = Vector3(0.0f, 1.5f, 0.0f);
	// 頭の回転制限角度（X軸方向）
	float _headRotationLimitX = 45.0f;

	bool		_callCancelEvent = false;
	bool		_callFireBreath = false;
	bool		_callFireBall = false;
	bool		_callLookAtTarget = false;
};

#pragma region ベースステート
class WyvernHSB : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernHSB(WyvernStateMachine* stateMachine,
		const std::string& animationName,
		float blendSeconds,
		bool isLoop,
		bool isUsingRootMotion) :
		HierarchicalStateBase(stateMachine),
		_animationName(animationName),
		_blendSeconds(blendSeconds),
		_isLoop(isLoop),
		_isUsingRootMotion(isUsingRootMotion)
	{
	}
	~WyvernHSB() override {}
	virtual void OnEnter() override;
	virtual void OnExecute(float elapsedTime) override {}
	virtual void OnExit() override {}
private:
	std::string _animationName = "";
	float		_blendSeconds = 0.2f;
	bool		_isLoop = false;
	bool		_isUsingRootMotion = false;
};

// アニメーション再生のみの簡易サブステート
class WyvernSSB : public StateBase<WyvernStateMachine>
{
public:
	WyvernSSB(WyvernStateMachine* stateMachine,
		const std::string& name,
		const std::string& animationName,
		float blendSeconds,
		bool isLoop,
		bool isUsingRootMotion) :
		StateBase(stateMachine),
		_name(name),
		_animationName(animationName),
		_blendSeconds(blendSeconds),
		_isLoop(isLoop),
		_isUsingRootMotion(isUsingRootMotion)
	{
	}
	const char* GetName() const override { return _name.c_str(); }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override {}
	void OnExit() override {}
private:
	std::string _name = "";
	std::string _animationName = "";
	float		_blendSeconds = 0.2f;
	bool		_isLoop = false;
	bool		_isUsingRootMotion = false;
};
#pragma endregion
