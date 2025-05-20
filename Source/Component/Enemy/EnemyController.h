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

protected:
    std::weak_ptr<CharactorController> _charactorController;
    std::weak_ptr<Animator> _animator;

    float _ATK = 1.0f;
};