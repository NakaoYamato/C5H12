#include "LockOnCamera.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Input/Input.h"
#include "../../Library/Graphics/Graphics.h"

#include <imgui.h>

// 開始時処理
void LockOnCamera::OnStart()
{
	_inputManager = GetActor()->GetScene()->GetActorManager().FindByClass<InputManager>(ActorTag::System);
}

// 起動時関数
void LockOnCamera::OnEntry()
{
	_targetScreenPosition = Vector2::Zero;
	// 現在の注視点と視点を取得
    _currentFocus = GetActor()->GetScene()->GetMainCamera()->GetFocus();
	_currentEye = GetActor()->GetScene()->GetMainCamera()->GetEye();

}

// 更新時処理
void LockOnCamera::OnUpdate(float elapsedTime)
{
	if (!_target || !_playerActor)
	{
		return;
	}

    // 入力情報を取得
    float moveX = _INPUT_VALUE("AxisRX") * _horizontalMovePower * elapsedTime;
    float moveY = _INPUT_VALUE("AxisRY") * _verticalMovePower * elapsedTime;

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

    if (auto inputManager = _inputManager.lock())
    {
        // カメラを動かせないなら入力値を無効化
        if (!inputManager->CanMoveCamera())
        {
            moveX = 0.0f;
            moveY = 0.0f;
        }
    }

    // Y軸回転
    angle.y += moveX * 0.5f;
    // X軸回転
    angle.x -= moveY * 0.5f;

	// ターゲットが画面外に出ているか確認
	Vector3 targetScreenPos = _target->GetActor()->GetTransform().GetWorldPosition().Project(
		Graphics::Instance().GetScreenWidth(),
		Graphics::Instance().GetScreenHeight(),
		GetActor()->GetScene()->GetMainCamera()->GetView(),
		GetActor()->GetScene()->GetMainCamera()->GetProjection());
    // 裏側の時
    if (targetScreenPos.z < 0.0f)
    {
        // 画面左側
        if (targetScreenPos.x < Graphics::Instance().GetScreenWidth() / 2.0f)
            // 現在の前方向から左回転
			angle.y -= DirectX::XMConvertToRadians(_behindTargetHorizontalMovePower);
        else
			// 現在の前方向から右回転
			angle.y += DirectX::XMConvertToRadians(_behindTargetHorizontalMovePower);
    }
    else
    {
		if (targetScreenPos.x < 0.0f)
            // 画面左外
			angle.y -= DirectX::XMConvertToRadians(_outOfScreenHorizontalMovePower);
		else if (targetScreenPos.x > Graphics::Instance().GetScreenWidth())
			// 画面右外
			angle.y += DirectX::XMConvertToRadians(_outOfScreenHorizontalMovePower);

		if (targetScreenPos.y < 0.0f)
			// 画面上外
			angle.x -= DirectX::XMConvertToRadians(_outOfScreenVerticalMovePower);
		else if (targetScreenPos.y > Graphics::Instance().GetScreenHeight())
			// 画面下外
			angle.x += DirectX::XMConvertToRadians(_outOfScreenVerticalMovePower);
    }

    if (angle.y > DirectX::XM_PI)
        angle.y -= DirectX::XM_2PI;
    else if (angle.y < -DirectX::XM_PI)
        angle.y += DirectX::XM_2PI;
    if (angle.x > _angleXLimitHigh)
        angle.x = _angleXLimitHigh;
    else if (angle.x < _angleXLimitLow)
        angle.x = _angleXLimitLow;

    Vector3 newFocus = _playerActor->GetTransform().GetPosition();
    // オフセット処理
    newFocus.y += _playerOffset;
    newFocus.x += right.x * _focusHorizontalOffset;
    newFocus.z += right.z * _focusHorizontalOffset;

    // 注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
    Vector3 newEye = newFocus - front * _cameraDistance;

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

    GetActor()->GetScene()->GetMainCamera()->SetLookAt(eye, focus, Vector3::Up);

    GetActor()->GetTransform().SetAngle(angle);
    _currentFocus = focus;
    _currentEye = eye;
    GetActor()->GetTransform().SetPosition(eye);
    // マウスの位置を画面内に修正
    _Mouse->ClipCursorInWindow();
    _Mouse->UpdatePosition();
}

// GUI描画
void LockOnCamera::DrawGui()
{
    ImGui::DragFloat(u8"水平入力補正値", &_horizontalMovePower, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"垂直入力補正値", &_verticalMovePower, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"カメラの半径", &_cameraRadius, 0.01f, 0.01f, 1.0f);
}

// ターゲット設定
void LockOnCamera::SetTarget(Targetable* target)
{
	_target = target;
}

// プレイヤーアクター設定
void LockOnCamera::SetPlayerActor(PlayerActor* playerActor)
{
	_playerActor = playerActor;
}
