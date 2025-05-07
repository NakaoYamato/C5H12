#pragma once

#include "Component.h"

class CharactorController : public Component
{
public:
    CharactorController() {}
    ~CharactorController() override {}
    // 名前取得
    const char* GetName() const override { return "CharactorController"; }

    // 更新処理
    void Update(float elapsedTime) override;
    // 固定間隔更新処理
    void FixedUpdate() override;
    // GUI描画
    void DrawGui() override;

    // 力を加える
    void AddForce(const Vector3& force)
    {
        acceleration += force;
    }

private:
    Vector3 velocity = {};
    Vector3 acceleration = {};

    Vector3 gravity = { 0.0f, -9.8f, 0.0f };
};