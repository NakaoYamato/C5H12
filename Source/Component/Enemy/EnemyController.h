#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Component/CharactorController.h"
#include "../../Library/Component/Animator.h"
#include "../../Source/Interface/Common/IDamagable.h"

class EnemyController : public Component, public IDamagable
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
    // GUI描画
    void DrawGui() override;
    // 衝突処理
    void OnContact(CollisionData& collisionData)override;

	// ダメージを与える
    virtual void AddDamage(float damage, Vector3 hitPosition) override;

#pragma region アクセサ
	const Vector3& GetTargetPosition() const { return _targetPosition; }
	float GetATK() const { return _ATK; }
	float GetSearchRange() const { return _searchRange; }
	float GetAttackRange() const { return _attackRange; }

	void SetTargetPosition(const Vector3& targetPosition) { _targetPosition = targetPosition; }
	void SetATK(float atk) { _ATK = atk; }
	void SetSearchRange(float searchRange) { _searchRange = searchRange; }
	void SetAttackRange(float attackRange) { _attackRange = attackRange; }
#pragma endregion

protected:
    std::weak_ptr<CharactorController> _charactorController;
    std::weak_ptr<Animator> _animator;

	Vector3 _targetPosition = Vector3::Zero;
    float _ATK = 1.0f;
	float _searchRange = 30.0f;
    float _attackRange = 10.0f;
};