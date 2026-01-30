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
void LockOnCamera::OnActivate()
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
    auto inputManager = _inputManager.lock();
    if (!inputManager)
        return;
	auto mainCamera = GetActor()->GetScene()->GetMainCameraActor();

    float sensitivity = mainCamera->GetCameraSensitivity();
    // 入力情報を取得
    float moveX = _INPUT_VALUE("AxisRX") * sensitivity * _horizontalMovePower * elapsedTime;
    float moveY = _INPUT_VALUE("AxisRY") * sensitivity * _verticalMovePower * elapsedTime;
    // 反転処理
    if (mainCamera->IsInvertX())
		moveX = -moveX;
	if (mainCamera->IsInvertY())
		moveY = -moveY;
    // カメラを動かせないなら入力値を無効化
    if (!inputManager->CanMoveCamera())
    {
        moveX = 0.0f;
        moveY = 0.0f;
    }

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

	// ターゲットの目標画面位置を更新
	_targetScreenPosition.x -= moveX;
	_targetScreenPosition.y -= moveY;
    // 制限
	_targetScreenPosition.x = MathF::Clamp(_targetScreenPosition.x, _targetScreenMinPosition.x, _targetScreenMaxPosition.x);
	_targetScreenPosition.y = MathF::Clamp(_targetScreenPosition.y, _targetScreenMinPosition.y, _targetScreenMaxPosition.y);

    // プレイヤーの画面位置を取得
    Vector3 playerScreenPos = _playerActor->GetTransform().GetWorldPosition().Project(
        Graphics::Instance().GetScreenWidth(),
        Graphics::Instance().GetScreenHeight(),
        GetActor()->GetScene()->GetMainCamera()->GetView(),
        GetActor()->GetScene()->GetMainCamera()->GetProjection());

	// ターゲットの画面位置を取得
	Vector3 currentTargetScreenPos = _target->GetActor()->GetTransform().GetWorldPosition().Project(
		Graphics::Instance().GetScreenWidth(),
		Graphics::Instance().GetScreenHeight(),
		GetActor()->GetScene()->GetMainCamera()->GetView(),
		GetActor()->GetScene()->GetMainCamera()->GetProjection());
    // ターゲットがプレイヤーより前にいる場合
    if (currentTargetScreenPos.z < playerScreenPos.z)
    {
        // 画面左側
        if (currentTargetScreenPos.x < Graphics::Instance().GetScreenWidth() / 2.0f)
            // 現在の前方向から左回転
			angle.y -= DirectX::XMConvertToRadians(_behindTargetHorizontalMovePower) * elapsedTime;
        else
			// 現在の前方向から右回転
			angle.y += DirectX::XMConvertToRadians(_behindTargetHorizontalMovePower) * elapsedTime;
    }
    else
    {
        // 目標位置とのずれから回転量を計算
        Vector3 currentTargetNDC = Vector3(
            (currentTargetScreenPos.x / Graphics::Instance().GetScreenWidth()) * 2.0f - 1.0f,
            -((currentTargetScreenPos.y / Graphics::Instance().GetScreenHeight()) * 2.0f - 1.0f),
            0.0f);
        Vector3 screenPosDiff = currentTargetNDC - Vector3(_targetScreenPosition.x, _targetScreenPosition.y, 0.0f);
		if (screenPosDiff.x < -_targetScreenPositionTolerance)
            // 画面左外
			angle.y -= DirectX::XMConvertToRadians(_outOfScreenHorizontalMovePower) * elapsedTime;
		else if (screenPosDiff.x > _targetScreenPositionTolerance)
			// 画面右外
			angle.y += DirectX::XMConvertToRadians(_outOfScreenHorizontalMovePower) * elapsedTime;

		if (screenPosDiff.y < -_targetScreenPositionTolerance)
			// 画面上外
			angle.x += DirectX::XMConvertToRadians(_outOfScreenVerticalMovePower) * elapsedTime;
		else if (screenPosDiff.y > _targetScreenPositionTolerance)
			// 画面下外
			angle.x -= DirectX::XMConvertToRadians(_outOfScreenVerticalMovePower) * elapsedTime;
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
    Vector3 eye = Vector3::Lerp(_currentEye, newEye, _eyeLerpSpeed * elapsedTime);

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
    ImGui::DragFloat2(u8"ターゲット画面位置", &_targetScreenPosition.x, 0.01f, -1.0f, 1.0f);
    ImGui::DragFloat2(u8"ターゲット画面位置最小値", &_targetScreenMinPosition.x, 0.01f, -1.0f, 1.0f);
    ImGui::DragFloat2(u8"ターゲット画面位置最大値", &_targetScreenMaxPosition.x, 0.01f, -1.0f, 1.0f);
    ImGui::DragFloat(u8"ターゲット画面位置許容範囲", &_targetScreenPositionTolerance, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat(u8"ターゲット裏側時水平回転速度(度)", &_behindTargetHorizontalMovePower, 0.1f, 0.0f, 100.0f, "%.1f", ImGuiSliderFlags_None);
    ImGui::DragFloat(u8"ターゲット画面外時水平回転速度(度)", &_outOfScreenHorizontalMovePower, 0.1f, 0.0f, 100.0f, "%.1f", ImGuiSliderFlags_None);
    ImGui::DragFloat(u8"ターゲット画面外時垂直回転速度(度)", &_outOfScreenVerticalMovePower, 0.1f, 0.0f, 100.0f, "%.1f", ImGuiSliderFlags_None);

    ImGui::Separator();
    ImGui::DragFloat(u8"プレイヤー注視点オフセット", &_playerOffset, 0.01f, -10.0f, 10.0f);
    ImGui::DragFloat(u8"注視点水平オフセット", &_focusHorizontalOffset, 0.01f, -10.0f, 10.0f);
    ImGui::DragFloat(u8"カメラ距離", &_cameraDistance, 0.1f, 0.1f, 100.0f);


    ImGui::DragFloat(u8"水平入力補正値", &_horizontalMovePower, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"垂直入力補正値", &_verticalMovePower, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"カメラの半径", &_cameraRadius, 0.01f, 0.01f, 1.0f);

    ImGui::Separator();
    ImGui::Checkbox(u8"ターゲット位置デバッグ表示", &_isDrawTargetDebug);
}

// デバッグ表示
void LockOnCamera::DebugRender(const RenderContext& rc)
{
    if (_isDrawTargetDebug)
    {
        Vector2 targetScreenPos{};
        targetScreenPos.x = (_targetScreenPosition.x * 0.5f + 0.5f) * Graphics::Instance().GetScreenWidth();
        targetScreenPos.y = (-_targetScreenPosition.y * 0.5f + 0.5f) * Graphics::Instance().GetScreenHeight();
        GetActor()->GetScene()->GetPrimitive()->Circle(
            rc.deviceContext,
            targetScreenPos,
            _debugTargetSize,
            Vector2::One,
            0.0f,
            Vector4::Red);
    }
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
