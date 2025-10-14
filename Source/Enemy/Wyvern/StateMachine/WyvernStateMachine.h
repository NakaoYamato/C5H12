#pragma once

#include <variant>
#include "../../Library/Component/StateController.h"
#include "../../Library/Math/Vector.h"
#include "../../Source/Common/Damageable.h"

// 前方宣言
class EnemyController;
class WyvernController;
class Animator;

class WyvernStateMachine : public StateMachine
{
public:
	WyvernStateMachine(
		EnemyController* enemy,
		WyvernController* wyvern,
		Animator* animator,
		Damageable* damageable);
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
