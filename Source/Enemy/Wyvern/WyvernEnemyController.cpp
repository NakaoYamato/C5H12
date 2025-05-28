#include "WyvernEnemyController.h"

#include <imgui.h>

// 開始処理
void WyvernEnemyController::Start()
{
	EnemyController::Start();

	_charactorController.lock()->SetRadius(2.5f);
	// ステートマシン作成
	_stateMachine = std::make_unique<WyvernStateMachine>(this, _animator.lock().get());
}

// 更新処理
void WyvernEnemyController::Update(float elapsedTime)
{
	// ステートマシンの実行
	_stateMachine->Execute(elapsedTime);
}
// GUI描画
void WyvernEnemyController::DrawGui()
{
	EnemyController::DrawGui();
	_stateMachine->DrawGui();
}

// ダメージを受けた時の処理
void WyvernEnemyController::OnDamage(float damage, Vector3 hitPosition)
{
	_damageCounter += damage;
	if (_damageCounter >= _damageReactionRate)
	{
		// ダメージリアクションを行う
		SetPerformDamageReaction(true);
		_damageCounter = 0.0f;
	}
}

#pragma region ネットワーク用
void WyvernEnemyController::ChangeState(const char* mainStateName, const char* subStateName)
{
	if (!_stateMachine)
		return;
	_stateMachine->ChangeState(mainStateName, subStateName);
}

const char* WyvernEnemyController::GetStateName()
{
	if (!_stateMachine)
		return "";
	return _stateMachine->GetStateName();
}

const char* WyvernEnemyController::GetSubStateName()
{
	if (!_stateMachine)
		return "";
	return _stateMachine->GetSubStateName();
}
#pragma endregion
