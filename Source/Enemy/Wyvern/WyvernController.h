#pragma once

#include "../../Library/Component/Component.h"
#include "../EnemyController.h"
#include "../../Source/AI/MetaAI.h"
#include "../../Library/Component/BehaviorController.h"

class WyvernController : public Component
{
public:
	WyvernController() {}
	~WyvernController() override {}
	// 名前取得
	const char* GetName() const override { return "WyvernController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

#pragma region アクセサ
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
	std::weak_ptr<EnemyController> _enemyController;
	// ビヘイビアコントローラー
	std::weak_ptr<BehaviorController> _behaviorController;
	// メタAI
	std::weak_ptr<MetaAI> _metaAI;
	// 近接攻撃範囲
	float _nearAttackRange = 10.0f;
	// 近接攻撃ができる角度
	float _nearAttackRadian = DirectX::XMConvertToRadians(40.0f);
	// 回転速度
	float _rotationSpeed = 1.0f;
	// ターゲットに向く角度
	float _lookAtRadian = DirectX::XMConvertToRadians(20.0f);
};