#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Component/CharactorController.h"
#include "../../Source/Common/Damageable.h"
#include "../../Library/Component/Animator.h"
#include "../../Library/Component/Effekseer/EffekseerEffectController.h"

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
    // ターゲット方向を向く
	virtual void LookAtTarget(const Vector3& target, float elapsedTime, float rotationSpeed);
#pragma region アクセサ
	std::shared_ptr<CharactorController> GetCharactorController() const { return _charactorController.lock(); }
#pragma region ネットワーク用
    // ステート変更
    virtual void ChangeState(const char* mainStateName, const char* subStateName) {};
    // ステート名取得
    virtual const char* GetStateName() { return ""; }
    // サブステート名取得
    virtual const char* GetSubStateName() { return ""; }
#pragma endregion

	const Vector3& GetTargetPosition() const { return _targetPosition; }

	float GetATK() const { return _ATK; }
	float GetSearchRange() const { return _searchRange; }
	float GetAttackRange() const { return _attackRange; }
	bool IsPerformDamageReaction() const { return _performDamageReaction; }

	void SetTargetPosition(const Vector3& targetPosition) { _targetPosition = targetPosition; }
	void SetATK(float atk) { _ATK = atk; }
	void SetSearchRange(float searchRange) { _searchRange = searchRange; }
	void SetAttackRange(float attackRange) { _attackRange = attackRange; }
	void SetPerformDamageReaction(bool performDamageReaction) { _performDamageReaction = performDamageReaction; }
#pragma endregion

protected:
    std::weak_ptr<CharactorController> _charactorController;
    std::weak_ptr<Animator> _animator;
    std::weak_ptr<EffekseerEffectController> _hitEffectController;
    std::weak_ptr<Damageable> _damageable;

	Vector3 _targetPosition = Vector3::Zero;
    float _ATK = 1.0f;
	float _searchRange = 30.0f;
    float _attackRange = 20.0f;

    // ダメージリアクションを行うかどうか
	bool _performDamageReaction = false;
};