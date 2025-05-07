#include "PlayerCameraController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Input/Input.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

#include <imgui.h>

void PlayerCameraController::Start()
{
}

void PlayerCameraController::Update(float elapsedTime)
{
    // F4を押していたらデバッグ用カメラ起動中
    if (Debug::Input::IsActive(DebugInput::BTN_F4))
        return;

    Vector3 angle = GetActor()->GetTransform().GetRotation();
    // カメラ回転値を回転行列に変換
    DirectX::XMMATRIX Transform =
        DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

    // 回転行列から右方向、上方向、前方向ベクトルを取り出す
    DirectX::XMVECTOR Right = Transform.r[0];
    DirectX::XMVECTOR Up = Transform.r[1];
    DirectX::XMVECTOR Front = Transform.r[2];
    Vector3 front{}, right{}, up{};
    DirectX::XMStoreFloat3(&front, Front);
    DirectX::XMStoreFloat3(&right, Right);
    DirectX::XMStoreFloat3(&up, Up);

    // 入力情報を取得
    float moveX = _INPUT_IS_AXIS("AxisRX") * _horizontalMovePower * elapsedTime;
    float moveY = _INPUT_IS_AXIS("AxisRY") * _verticalMovePower * elapsedTime;

    // Y軸回転
    angle.y += moveX * 0.5f;
    if (angle.y > DirectX::XM_PI)
        angle.y -= DirectX::XM_2PI;
    else if (angle.y < -DirectX::XM_PI)
        angle.y += DirectX::XM_2PI;
    // X軸回転
    angle.x -= moveY * 0.5f;
    if (angle.x > DirectX::XMConvertToRadians(89.9f))
        angle.x = DirectX::XMConvertToRadians(89.9f);
    else if (angle.x < -DirectX::XMConvertToRadians(89.9f))
        angle.x = -DirectX::XMConvertToRadians(89.9f);
    GetActor()->GetTransform().SetRotation(angle);

    Vector3 target = _playerActor->GetTransform().GetPosition();
    target.y += _cameraOffsetY;

    // 注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
    Vector3 eye = target - front * _cameraDistance;

    GetActor()->GetScene()->GetMainCamera()->SetLookAt(eye, target, up);
}

void PlayerCameraController::DrawGui()
{
    ImGui::DragFloat("CameraDistance", &_cameraDistance, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat("CameraOffsetY", &_cameraOffsetY, 0.1f, -10.0f, 10.0f);
    ImGui::DragFloat("HorizontalMovePower", &_horizontalMovePower, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat("VerticalMovePower", &_verticalMovePower, 0.1f, 0.0f, 100.0f);
}
