#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Camera/Camera.h"

#include "../../Source/Actor/Player/PlayerActor.h"

class PlayerCameraController : public Component
{
public:
    PlayerCameraController(PlayerActor* playerActor) : _playerActor(playerActor) {}
    ~PlayerCameraController() override {}
    // 名前取得
    const char* GetName() const override { return "PlayerCameraController"; }
    // 開始処理
    void Start() override;
    // 更新処理
    void Update(float elapsedTime) override;
    // GUI描画
    void DrawGui() override;

private:
    PlayerActor* _playerActor;

    float _cameraOffsetY = 1.0f;

    float _cameraDistance = 5.0f;
    float _horizontalMovePower = 6.0f;
    float _verticalMovePower = 3.0f;
};