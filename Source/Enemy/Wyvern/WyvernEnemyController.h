#pragma once

#include "../EnemyController.h"
#include "StateMachine/WyvernStateMachine.h"

class WyvernEnemyController : public EnemyController
{
public:
	WyvernEnemyController() {}
	~WyvernEnemyController() override {}
	// 名前取得
	const char* GetName() const override { return "WyvernEnemyController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 遅延更新処理
	void LateUpdate(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

#pragma region ネットワーク用
	// ステート変更
	virtual void ChangeState(const char* mainStateName, const char* subStateName) override;
	// ステート名取得
	virtual const char* GetStateName() override;
	// サブステート名取得
	virtual const char* GetSubStateName() override;
#pragma endregion

#pragma region アクセサ
	WyvernStateMachine* GetState() const { return _stateMachine.get(); }

	float GetNearAttackRange() const { return _nearAttackRange; }
	float GetNearAttackRadian() const { return _nearAttackRadian; }
	float GetRotationSpeed() const { return _rotationSpeed; }
	float GetLookAtRadian() const { return _lookAtRadian; }

	void SetNearAttackRange(float nearAttackRange) { _nearAttackRange = nearAttackRange; }
	void SetNearAttackRadian(float nearAttackRadian) { _nearAttackRadian = nearAttackRadian; }
	void SetRotationSpeed(float rotationSpeed) { _rotationSpeed = rotationSpeed; }
	void SetLookAtRadian(float lookAtRadian) { _lookAtRadian = lookAtRadian; }
#pragma endregion

private:
	// ステートマシン
	std::unique_ptr<WyvernStateMachine> _stateMachine;

	// 近接攻撃範囲
	float _nearAttackRange = 10.0f;
	// 近接攻撃ができる角度
	float _nearAttackRadian = DirectX::XMConvertToRadians(40.0f);
	// 回転速度
	float _rotationSpeed = 1.0f;
	// ターゲットに向く角度
	float _lookAtRadian = DirectX::XMConvertToRadians(20.0f);

	// ダメージリアクションの間隔
	float _damageReactionRate = 2.0f;
	// ダメージカウンター
	float _damageCounter = 0.0f;
};