#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Camera/Camera.h"

#include "../../Source/Actor/Player/PlayerActor.h"

/// <summary>
/// プレイヤーカメラコントローラー
/// Transform.Position == eye
/// Transform.Rotation == camera angle
/// </summary>
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
	// 固定間隔更新処理
    void FixedUpdate() override;
    // GUI描画
    void DrawGui() override;

private:
    PlayerActor* _playerActor;

    Vector3 _currentFocus{};

    float _cameraOffsetY = 1.0f;
	float _focusLerpSpeed = 5.0f;
	float _eyeLerpSpeed = 1.0f;

    float _cameraDistance = 5.0f;
    float _horizontalMovePower = 6.0f;
    float _verticalMovePower = 3.0f;
};