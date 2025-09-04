#pragma once

#include "../../Library/Component/Component.h"
#include "../EnemyController.h"

class GryphusController : public Component
{
public:
    GryphusController() {}
    ~GryphusController() override {}
    // 名前取得
    const char* GetName() const override { return "GryphusController"; }
    // 開始処理
    void Start() override;
    // 更新処理
    void Update(float elapsedTime) override;
    // GUI描画
    void DrawGui() override;

#pragma region アクセサ
    // ジョグ移動速度取得
    float GetJogSpeed() const { return _jogSpeed; }
    // ジョグ移動速度設定
    void SetJogSpeed(float jogSpeed) { _jogSpeed = jogSpeed; }
    // ダッシュ移動速度取得
    float GetDashSpeed() const { return _dashSpeed; }
    // ダッシュ移動速度設定
    void SetDashSpeed(float dashSpeed) { _dashSpeed = dashSpeed; }
#pragma endregion
private:
    std::weak_ptr<EnemyController> _enemyController;
    // ジョグ移動速度
    float _jogSpeed = 3.0f;
    // ダッシュ移動速度
    float _dashSpeed = 8.0f;
};