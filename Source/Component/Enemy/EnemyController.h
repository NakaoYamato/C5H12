#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Component/CharactorController.h"

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
    // GUI描画
    void DrawGui() override;
    // 衝突処理
    void OnContact(CollisionData& collisionData)override;
};