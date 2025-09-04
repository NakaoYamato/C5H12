#include "PlayerCameraController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Input/Input.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

#include <imgui.h>

void PlayerCameraController::Start()
{
    Vector3 angle = GetActor()->GetTransform().GetRotation();
    // カメラ回転値を回転行列に変換
    DirectX::XMMATRIX Transform =
        DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

    // 回転行列から前方向ベクトルを取り出す
    DirectX::XMVECTOR Front = Transform.r[2];
    Vector3 front{};
    DirectX::XMStoreFloat3(&front, Front);
    // 初期設定
    _currentFocus = _playerActor->GetTransform().GetPosition();
    _currentFocus.y += _cameraOffsetY;
    GetActor()->GetTransform().SetPosition(_currentFocus - front * _cameraDistance);

	_focusNodeIndex = _playerActor->GetModel().lock()->GetNodeIndex("pelvis");
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
    float moveX = _INPUT_VALUE("AxisRX") * _horizontalMovePower * elapsedTime;
    float moveY = _INPUT_VALUE("AxisRY") * _verticalMovePower * elapsedTime;

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
    GetActor()->GetTransform().SetAngle(angle);

    //Vector3 newFocus = _playerActor->GetTransform().GetPosition();
    auto& focusNode = _playerActor->GetModel().lock()->GetPoseNodes().at(_focusNodeIndex);
    Vector3 newFocus = Vector3::TransformCoord(focusNode.position, focusNode.parent->worldTransform);
    newFocus.y += _cameraOffsetY;

    // 注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
    Vector3 newEye = newFocus - front * _cameraDistance;

    // 新しい視点とステージの当たり判定
    float distance = _cameraDistance;
    Vector3 hitPosition{}, hitNormal{};
    Actor* hitActor = nullptr;
	if (GetActor()->GetScene()->GetCollisionManager().SphereCast(
        newFocus, -front.Normalize(),
        _cameraRadius, 
        &distance,
        &hitPosition,
        &hitNormal,
        &hitActor))
	{
        // 密接していなければdistanceを使う
        if (distance > 0.0f)
            newEye = newFocus - front * distance;
	}

    // 補完処理
    Vector3 focus = Vector3::Lerp(_currentFocus, newFocus, _focusLerpSpeed * elapsedTime);
    Vector3 eye = Vector3::Lerp(GetActor()->GetTransform().GetPosition(), newEye, _eyeLerpSpeed * elapsedTime);
    //eye = newEye;

    GetActor()->GetScene()->GetMainCamera()->SetLookAt(eye, focus, Vector3::Up);

    _currentFocus = focus;
    GetActor()->GetTransform().SetPosition(eye);
	// マウスの位置を画面内に修正
    _Mouse->ClipCursorInWindow();
    _Mouse->UpdatePosition();
}

// 固定間隔更新処理
void PlayerCameraController::FixedUpdate()
{
}

void PlayerCameraController::DrawGui()
{
    ImGui::DragFloat(u8"カメラ距離", &_cameraDistance, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"オフセット", &_cameraOffsetY, 0.1f, -10.0f, 10.0f);
    ImGui::DragFloat(u8"注視点補完速度", &_focusLerpSpeed, 0.1f, 0.01f, 20.0f);
    ImGui::DragFloat(u8"視点補完速度", &_eyeLerpSpeed, 0.1f, 0.01f, 20.0f);
    ImGui::DragFloat(u8"水平入力補正値", &_horizontalMovePower, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"垂直入力補正値", &_verticalMovePower, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"カメラの半径", &_cameraRadius, 0.01f, 0.01f, 1.0f);
}
