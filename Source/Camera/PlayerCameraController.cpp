#include "PlayerCameraController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Input/Input.h"

#include <imgui.h>

// 開始時処理
void PlayerCameraController::OnStart()
{
	_inputManager = GetActor()->GetScene()->GetActorManager().FindByClass<InputManager>(ActorTag::System);
}

// 起動時関数
void PlayerCameraController::OnActivate()
{
    // 現在の注視点と視点を取得
    _currentFocus = GetActor()->GetScene()->GetMainCamera()->GetFocus();
    _currentEye = GetActor()->GetScene()->GetMainCamera()->GetEye();
}

void PlayerCameraController::DrawGui()
{
    CameraControllerBase::DrawGui();
    ImGui::Separator();

	ImGui::DragFloat(u8"X軸上限角度(度)", &_angleXLimitHigh, 1.0f, -90.0f, 90.0f, "%.1f", ImGuiSliderFlags_None);
	ImGui::DragFloat(u8"X軸下限角度(度)", &_angleXLimitLow, 1.0f, -90.0f, 90.0f, "%.1f", ImGuiSliderFlags_None);

	ImGui::DragFloat(u8"見上げ開始角度(度)", &_lookingUpStartAngle, 1.0f, -90.0f, 90.0f, "%.1f", ImGuiSliderFlags_None);
	ImGui::DragFloat(u8"見上げ角度値(度)", &_lookingUpAngleValue, 1.0f, 0.0f, 90.0f, "%.1f", ImGuiSliderFlags_None);

    ImGui::DragFloat(u8"カメラ距離", &_cameraDistance, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"垂直方向のオフセット", &_focusVerticalOffset, 0.01f, -10.0f, 10.0f);
    ImGui::DragFloat(u8"水平方向のオフセット", &_focusHorizontalOffset, 0.01f, -10.0f, 10.0f);
    ImGui::DragFloat(u8"注視点補完速度", &_focusLerpSpeed, 0.1f, 0.01f, 20.0f);
    ImGui::DragFloat(u8"視点補完速度", &_eyeLerpSpeed, 0.1f, 0.01f, 20.0f);
    ImGui::DragFloat(u8"水平入力補正値", &_horizontalMovePower, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"垂直入力補正値", &_verticalMovePower, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"カメラの半径", &_cameraRadius, 0.01f, 0.01f, 1.0f);
    ImGui::Separator();

	ImGui::DragFloat(u8"抜刀時注視点垂直オフセット", &_combatFocusVerticalOffset, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat(u8"抜刀時注視点水平オフセット", &_combatFocusHorizontalOffset, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat(u8"抜刀時注視点補完速度", &_combatFocusLerpSpeed, 0.1f, 0.01f, 20.0f);
	ImGui::DragFloat(u8"抜刀時視点補完速度", &_combatEyeLerpSpeed, 0.1f, 0.01f, 20.0f);
	ImGui::DragFloat(u8"抜刀時カメラ距離", &_cameraDistance, 0.1f, 0.0f, 100.0f);
}

// 更新時処理
void PlayerCameraController::OnUpdate(float elapsedTime)
{
	if (!_playerActor)
	{
		return;
	}

    // 入力情報を取得
    float moveX = _INPUT_VALUE("AxisRX") * _horizontalMovePower * elapsedTime;
    float moveY = _INPUT_VALUE("AxisRY") * _verticalMovePower * elapsedTime;

    if (auto inputManager = _inputManager.lock())
    {
        // カメラを動かせないなら入力値を無効化
        if (!inputManager->CanMoveCamera())
        {
            moveX = 0.0f;
            moveY = 0.0f;
        }
    }

    // フェード中は動かせないようにする
	if (GetActor()->GetScene()->GetFade()->IsFading())
		return;

    // 抜刀状態か確認
    bool isCombat = false;
    {
        if (std::string(_stateController.lock()->GetStateName()).find("Combat") != std::string::npos)
            isCombat = true;
    }

    float focusVerticalOffset = isCombat ? _combatFocusVerticalOffset : _focusVerticalOffset;
    float focusHorizontalOffset = isCombat ? _combatFocusHorizontalOffset : _focusHorizontalOffset;
    float focusLerpSpeed = isCombat ? _combatFocusLerpSpeed : _focusLerpSpeed;
    float eyeLerpSpeed = isCombat ? _combatEyeLerpSpeed : _eyeLerpSpeed;
    float cameraDistance = isCombat ? _combatCameraDistance : _cameraDistance;

    Vector3 angle = GetActor()->GetTransform().GetAngle();
    // カメラ回転値を回転行列に変換
    DirectX::XMMATRIX Transform =
        DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

    // 回転行列から右方向、上方向、前方向ベクトルを取り出す
    DirectX::XMVECTOR Right = Transform.r[0];
    DirectX::XMVECTOR Up = Transform.r[1];
    DirectX::XMVECTOR Front = Transform.r[2];
    Vector3 front{}, right{}, up{};
    DirectX::XMStoreFloat3(&front, DirectX::XMVector3Normalize(Front));
    DirectX::XMStoreFloat3(&right, DirectX::XMVector3Normalize(Right));
    DirectX::XMStoreFloat3(&up, DirectX::XMVector3Normalize(Up));

    // Y軸回転
    angle.y += moveX * 0.5f;
    if (angle.y > DirectX::XM_PI)
        angle.y -= DirectX::XM_2PI;
    else if (angle.y < -DirectX::XM_PI)
        angle.y += DirectX::XM_2PI;
    // X軸回転
    angle.x -= moveY * 0.5f;
    if (angle.x > _angleXLimitHigh)
        angle.x = _angleXLimitHigh;
    else if (angle.x < _angleXLimitLow)
        angle.x = _angleXLimitLow;

    Vector3 newFocus = _playerActor->GetTransform().GetPosition();
    // オフセット処理
    newFocus.y += focusVerticalOffset;
    // 下から見るときはオフセットの影響を薄くする
    float lookingUpValue = angle.x - _lookingUpStartAngle;
    float horizontalRate = lookingUpValue < 0.0f ?
        1.0f - std::clamp(std::fabsf(lookingUpValue / _lookingUpAngleValue), 0.0f, 1.0f) :
        1.0f;
    newFocus.x += horizontalRate * right.x * focusHorizontalOffset;
    newFocus.z += horizontalRate * right.z * focusHorizontalOffset;

    // 注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
    Vector3 newEye = newFocus - front * cameraDistance;

    if (auto cameraActor = GetActor()->GetScene()->GetMainCameraActor())
    {
        // カメライベント受信者が存在するならオフセットを加算
        const Vector3& eyeOffset = cameraActor->GetCallBackEyeOffset();
        // カメラの角度に合わせてオフセットを変換
        newEye += right * eyeOffset.x;
        newEye += up * eyeOffset.y;
        newEye += front * eyeOffset.z;
    }

    // 新しい視点とステージの当たり判定
    float distance = cameraDistance;
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
    Vector3 focus = Vector3::Lerp(_currentFocus, newFocus, focusLerpSpeed * elapsedTime);
    Vector3 eye = Vector3::Lerp(_currentEye, newEye, eyeLerpSpeed * elapsedTime);

    GetActor()->GetScene()->GetMainCamera()->SetLookAt(eye, focus, Vector3::Up);

    GetActor()->GetTransform().SetAngle(angle);
    _currentFocus = focus;
    _currentEye = eye;
    GetActor()->GetTransform().SetPosition(eye);
    // マウスの位置を画面内に修正
    _Mouse->ClipCursorInWindow();
    _Mouse->UpdatePosition();
}

// プレイヤーアクター設定
void PlayerCameraController::SetPlayerActor(PlayerActor* playerActor)
{
	_playerActor = playerActor;
    _stateController = _playerActor->GetComponent<StateController>();
    // 初期設定
    GetActor()->GetTransform().SetAngle(_stateController.lock()->GetActor()->GetTransform().GetAngle());

    Vector3 angle = GetActor()->GetTransform().GetAngle();
    // カメラ回転値を回転行列に変換
    DirectX::XMMATRIX Transform =
        DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

    // 回転行列から前方向ベクトルを取り出す
    DirectX::XMVECTOR Front = Transform.r[2];
    Vector3 front{};
    DirectX::XMStoreFloat3(&front, DirectX::XMVector3Normalize(Front));

    _currentFocus = _playerActor->GetTransform().GetPosition();
    _currentFocus.y += _focusVerticalOffset;
    _currentEye = _currentFocus - front * _cameraDistance;
    GetActor()->GetTransform().SetPosition(_currentEye);
    GetActor()->GetScene()->GetMainCamera()->SetLookAt(_currentEye, _currentFocus, Vector3::Up);
}

// リセット
void PlayerCameraController::Reset(const Vector3& angle)
{
    // カメラ回転値を回転行列に変換
    DirectX::XMMATRIX Transform =
        DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

    // 回転行列から右方向、上方向、前方向ベクトルを取り出す
    DirectX::XMVECTOR Right = Transform.r[0];
    DirectX::XMVECTOR Up = Transform.r[1];
    DirectX::XMVECTOR Front = Transform.r[2];
    Vector3 front{}, right{}, up{};
    DirectX::XMStoreFloat3(&front, DirectX::XMVector3Normalize(Front));
    DirectX::XMStoreFloat3(&right, DirectX::XMVector3Normalize(Right));
    DirectX::XMStoreFloat3(&up, DirectX::XMVector3Normalize(Up));

    Vector3 newFocus = _playerActor->GetTransform().GetPosition();
    // オフセット処理
    newFocus.y += _focusVerticalOffset;
    // 下から見るときはオフセットの影響を薄くする
    float lookingUpValue = angle.x - _lookingUpStartAngle;
    float horizontalRate = lookingUpValue < 0.0f ?
        1.0f - std::clamp(std::fabsf(lookingUpValue / _lookingUpAngleValue), 0.0f, 1.0f) :
        1.0f;
    newFocus.x += horizontalRate * right.x * _focusHorizontalOffset;
    newFocus.z += horizontalRate * right.z * _focusHorizontalOffset;

    // 注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
    Vector3 newEye = newFocus - front * _cameraDistance;

    GetActor()->GetScene()->GetMainCamera()->SetLookAt(newEye, newFocus, Vector3::Up);

    GetActor()->GetTransform().SetAngle(angle);
    _currentFocus = newFocus;
    _currentEye = newEye;
    GetActor()->GetTransform().SetPosition(_currentEye);
}
