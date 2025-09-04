#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Component/CharactorController.h"
#include "../../Library/Component/Effekseer/EffekseerEffectController.h"
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

    // 指定位置との角度
    float GetAngleToTarget(const Vector3& target);
	// ターゲット方向に移動
	void MoveToTarget(const Vector3& targetPosition, float speed);
    // ターゲット方向を向く
	virtual void LookAtTarget(const Vector3& target, float elapsedTime, float rotationSpeed);
	// ターゲットの範囲内かどうか
	bool IsInTargetRange(const Vector3& targetPosition, float targetRadius = 1.0f);
	// ターゲットの範囲内かどうか
	bool IsInTargetRange() { return IsInTargetRange(_targetPosition, _targetRadius); }
	// 攻撃クールタイム中かどうか
	bool IsAttackCooldown() const { return _attackCooldown > 0.0f; }
#pragma region アクセサ
	std::shared_ptr<CharactorController> GetCharactorController() const { return _charactorController.lock(); }
	const Vector3& GetTargetPosition() const { return _targetPosition; }

	float GetTargetRadius() const { return _targetRadius; }
	float GetATK() const { return _ATK; }
	float GetSearchRange() const { return _searchRange; }
	float GetCombatRange() const { return _combatRange; }
	float GetAttackRange() const { return _attackRange; }
	float GetNearAttackRange() const { return _nearAttackRange; }
	float GetRotationSpeed() const { return _rotationSpeed; }
	float GetLookAtRadian() const { return _lookAtRadian; }
	float GetAttackCooldown() const { return _attackCooldown; }

	bool IsPerformDamageReaction() const { return _performDamageReaction; }
	bool InFighting() const { return _inFighting; }

	void SetTargetPosition(const Vector3& targetPosition) { _targetPosition = targetPosition; }
	void SetTargetRadius(float targetRadius) { _targetRadius = targetRadius; }
	void SetTarget(const Vector3& targetPosition, float targetRadius = 1.0f){
		_targetPosition = targetPosition;
		_targetRadius = targetRadius;
	}
	void SetATK(float atk) { _ATK = atk; }
	void SetSearchRange(float searchRange) { _searchRange = searchRange; }
	void SetCombatRange(float combatRange) { _combatRange = combatRange; }
	void SetAttackRange(float attackRange) { _attackRange = attackRange; }
	void SetNearAttackRange(float nearAttackRange) { _nearAttackRange = nearAttackRange; }
	void SetRotationSpeed(float rotationSpeed) { _rotationSpeed = rotationSpeed; }
	void SetLookAtRadian(float lookAtRadian) { _lookAtRadian = lookAtRadian; }
	void SetAttackCooldown(float attackCooldown) { _attackCooldown = attackCooldown; }
	void SetPerformDamageReaction(bool performDamageReaction) { _performDamageReaction = performDamageReaction; }
	void SetInFighting(bool inFighting) { _inFighting = inFighting; }
#pragma endregion

protected:
    std::weak_ptr<CharactorController> _charactorController;
    std::weak_ptr<Animator> _animator;
    std::weak_ptr<EffekseerEffectController> _hitEffectController;
    std::weak_ptr<Damageable> _damageable;

	Vector3 _targetPosition = Vector3::Zero;
    // ターゲットの半径
	float _targetRadius = 1.0f;
	// 攻撃力
    float _ATK = 1.0f;
	// ターゲットの検索範囲
	float _searchRange = 30.0f;
	// 戦闘継続範囲
	float _combatRange = 60.0f;
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

    // 戦闘中かどうか
	bool _inFighting = false;
    // ダメージリアクションを行うかどうか
	bool _performDamageReaction = false;
    // ダメージリアクションの間隔
    float _damageReactionRate = 2.0f;
    // ダメージカウンター
    float _damageCounter = 0.0f;
};