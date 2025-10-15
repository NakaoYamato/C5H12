#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Component/CharactorController.h"
#include "../../Library/Component/EffectController.h"
#include "../../Library/Component/Animator.h"

#include "../../Source/Common/Damageable.h"

class EnemyController : public Component
{
public:
    EnemyController() {}
    ~EnemyController() override {}
    // 名前取得
    const char* GetName() const override { return "EnemyController"; }
    // 開始処理
    void Start() override;
    // 更新処理
    void Update(float elapsedTime) override;
    // 3D描画後の描画処理
    void DelayedRender(const RenderContext& rc) override;
    // GUI描画
    void DrawGui() override;
    // 接触時処理
    void OnContactEnter(CollisionData& collisionData) override;

    // ターゲットとの角度
    float GetAngleToTarget(const Vector3& target);
	// ターゲット方向に移動
	void MoveToTarget(const Vector3& targetPosition, float speed);
    // ターゲット方向を向く
	virtual void LookAtTarget(const Vector3& target, float elapsedTime, float rotationSpeed);
	// 攻撃クールタイム中かどうか
	bool IsAttackCooldown() const { return _attackCooldown > 0.0f; }
#pragma region アクセサ
	std::shared_ptr<CharactorController> GetCharactorController() const { return _charactorController.lock(); }

	float GetATK() const { return _ATK; }
	float GetAttackRange() const { return _attackRange; }
	float GetNearAttackRange() const { return _nearAttackRange; }
	float GetRotationSpeed() const { return _rotationSpeed; }
	float GetLookAtRadian() const { return _lookAtRadian; }
	float GetAttackCooldown() const { return _attackCooldown; }

	bool IsPerformDamageReaction() const { return _performDamageReaction; }

	void SetATK(float atk) { _ATK = atk; }
	void SetAttackRange(float attackRange) { _attackRange = attackRange; }
	void SetNearAttackRange(float nearAttackRange) { _nearAttackRange = nearAttackRange; }
	void SetRotationSpeed(float rotationSpeed) { _rotationSpeed = rotationSpeed; }
	void SetLookAtRadian(float lookAtRadian) { _lookAtRadian = lookAtRadian; }
	void SetAttackCooldown(float attackCooldown) { _attackCooldown = attackCooldown; }
	void SetPerformDamageReaction(bool performDamageReaction) { _performDamageReaction = performDamageReaction; }

	void SetDamageReactionRate(float rate) { _damageReactionRate = rate; }
#pragma endregion

protected:
    std::weak_ptr<CharactorController> _charactorController;
    std::weak_ptr<Animator> _animator;
    std::weak_ptr<EffectController> _effectController;
    std::weak_ptr<Damageable> _damageable;

	// 攻撃力
    float _ATK = 1.0f;
	// 攻撃範囲
    float _attackRange = 20.0f;
	// 近接攻撃範囲
	float _nearAttackRange = 10.0f;
	// 回転速度
	float _rotationSpeed = 1.0f;
	// ターゲットに向く角度
	float _lookAtRadian = DirectX::XMConvertToRadians(20.0f);
	// 攻撃クールタイム
	float _attackCooldown = 0.0f;

    // ダメージリアクションを行うかどうか
	bool _performDamageReaction = false;
    // ダメージリアクションの間隔
    float _damageReactionRate = 2.0f;
    // ダメージカウンター
    float _damageCounter = 0.0f;
};