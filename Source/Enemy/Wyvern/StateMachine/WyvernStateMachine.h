#pragma once

#include <variant>
#include "../../Library/Algorithm/StateMachine/StateMachine.h"
#include "../../Library/Math/Vector.h"
#include "../../Source/Common/Damageable.h"

// 前方宣言
class WyvernEnemyController;
class Animator;

class WyvernStateMachine
{
public:
	WyvernStateMachine(WyvernEnemyController* wyvern, Animator* animator, Damageable* damageable);
	~WyvernStateMachine() {}
	// 実行処理
	void Execute(float elapsedTime);
	// Gui描画
	void DrawGui();

#pragma region アクセサ
	StateMachine<WyvernStateMachine>& GetStateMachine() { return _stateMachine; }
	WyvernEnemyController* GetWyvern() { return _wyvern; }
	Animator* GetAnimator() { return _animator; }
	Damageable* GetDamageable() { return _damageable; }

	// ステート変更
	void ChangeState(const char* mainStateName, const char* subStateName);
	// ステート名取得
	const char* GetStateName();
	// サブステート名取得
	const char* GetSubStateName();

	// ブレス攻撃のグローバル位置を設定
	const Vector3& GetBreathGlobalPosition() const { return _breathGlobalPosition; }
	// キャンセルイベントを取得
	bool CallCancelEvent() const { return _callCancelEvent; }
	// ブレス攻撃イベントを取得
	bool CallFireBreathEvent() const { return _callFireBreath; }
	// ターゲットを見つめるイベントを取得
	bool CallLookAtTargetEvent() const { return _callLookAtTarget; }
#pragma endregion


private:
	StateMachine<WyvernStateMachine> _stateMachine;
	WyvernEnemyController* _wyvern = nullptr;
	Animator* _animator = nullptr;
	Damageable* _damageable = nullptr;

	// ブレス攻撃のグローバル位置
	Vector3 _breathGlobalPosition = Vector3::Zero;
	// 頭の回転時のターゲットオフセット
	Vector3 _headRotationOffset = Vector3(0.0f, 1.5f, 0.0f);
	// 頭の回転制限角度（X軸方向）
	float _headRotationLimitX = 45.0f;

	bool		_callCancelEvent = false;
	bool		_callFireBreath = false;
	bool		_callLookAtTarget = false;
};
