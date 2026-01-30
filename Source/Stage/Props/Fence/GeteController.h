#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Component/Collider/BoxCollider.h"

#include "../../Source/Quest/QuestController.h"

class GeteController : public Component
{
public:
    GeteController() {}
    ~GeteController() override {}
    // 名前取得
    const char* GetName() const override { return "GeteController"; }
    
    // 開始時処理
    void Start() override;
    // 更新処理
    void Update(float elapsedTime) override;
    // GUI描画
    void DrawGui() override;
    // オブジェクトとの接触時の処理
    void OnContact(CollisionData& collisionData) override;

private:
    std::weak_ptr<BoxCollider> _boxCollider;
    std::weak_ptr<QuestController> _questController;

    // 剛性モーメントの計算用
    float _forceY = 0.0f;
    float _angularVelocityY = 0.0f;
    // 回転の摩擦力
    float _friction = 1.0f;
    // 回転に対しての補正値
    float _rotationFactor = 10.0f;
    // 元に戻る力
    float _returnForce = 100.0f;
};