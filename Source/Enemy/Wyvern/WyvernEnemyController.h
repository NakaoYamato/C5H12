#pragma once

#include "../EnemyController.h"
#include "BehaviorTree/WyvernBehaviorTree.h"

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
	// GUI描画
	void DrawGui() override;

	// ダメージを与える
	void AddDamage(float damage, Vector3 hitPosition) override;

#pragma region アクセサ
	float GetNearAttackRange() const { return _nearAttackRange; }
	float GetNearAttackRadian() const { return _nearAttackRadian; }
	float GetRotationSpeed() const { return _rotationSpeed; }

	void SetNearAttackRange(float nearAttackRange) { _nearAttackRange = nearAttackRange; }
	void SetNearAttackRadian(float nearAttackRadian) { _nearAttackRadian = nearAttackRadian; }
	void SetRotationSpeed(float rotationSpeed) { _rotationSpeed = rotationSpeed; }
#pragma endregion

private:
	// ビヘイビアツリー
	std::unique_ptr<WyvernBehaviorTree> _behaviorTree;

	// 近接攻撃範囲
	float _nearAttackRange = 10.0f;
	// 近接攻撃ができる角度
	float _nearAttackRadian = DirectX::XMConvertToRadians(50.0f);
	// 回転速度
	float _rotationSpeed = 1.0f;
};