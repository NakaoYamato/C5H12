#include "WyvernEnemyController.h"

// 開始処理
void WyvernEnemyController::Start()
{
	EnemyController::Start();
	// ステートマシン初期化
	_stateMachine = std::make_unique<WyvernStateMachine>(this, _animator.lock().get());
}
// 更新処理
void WyvernEnemyController::Update(float elapsedTime)
{
	// ステートマシン実行
	_stateMachine->Execute(elapsedTime);
}
// ダメージを与える
void WyvernEnemyController::AddDamage(float damage, Vector3 hitPosition)
{
	EnemyController::AddDamage(damage, hitPosition);
}
