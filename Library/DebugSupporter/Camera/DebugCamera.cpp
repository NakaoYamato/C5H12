#include "DebugCamera.h"

#include "../../Library/Scene/SceneManager.h"
#include "../../Library/Input/Input.h"
#include "../DebugSupporter.h"
#include "../../Library/Math/Quaternion.h"

#include <imgui.h>

#define _ROTATION_EYE

// 更新処理
void DebugCamera::Update(float elapsedTime)
{
    // F4を押していたら起動
    if (Debug::Input::IsActive(DebugInput::BTN_F4))
    {
        // ローディング等でカメラがないときは処理しない
		auto mainCamera = SceneManager::Instance().GetCurrentScene()->GetMainCamera();
		if (mainCamera == nullptr)
			return;

        // 起動した瞬間なら現在の視点と角度を引き継ぐ
        if (!_isActive)
        {
            _eye = mainCamera->GetEye();
            DirectX::XMVECTOR S, R, T;
            DirectX::XMMatrixDecompose(&S, &R, &T, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&mainCamera->GetView())));
            _angle = Quaternion::ToRollPitchYaw(R);
        }

        // 使用フラグをオン
        _isActive = true;

        // ImGuiのウィンドウを選択していたら処理しない
#ifdef _DEBUG
		// シーンを描画しているImGuiウィンドウが選択されていない場合
        if (!SceneManager::Instance().GetCurrentScene()->IsImGuiSceneWindowSelected())
        {
            if (ImGui::IsWindowHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AnyWindow) ||
                ImGui::IsWindowFocused(ImGuiFocusedFlags_::ImGuiFocusedFlags_AnyWindow))
                return;
        }
#endif // _DEBUG

#ifdef _ROTATION_EYE
        // カメラ回転値を回転行列に変換
        DirectX::XMMATRIX Transform =
            DirectX::XMMatrixRotationRollPitchYaw(_angle.x, _angle.y, _angle.z);

        // 回転行列から右方向、上方向、前方向ベクトルを取り出す
        DirectX::XMVECTOR Right = Transform.r[0];
        DirectX::XMVECTOR Up = Transform.r[1];
        DirectX::XMVECTOR Front = Transform.r[2];
        Vector3 front{}, right{}, up{};
        DirectX::XMStoreFloat3(&front, Front);
        DirectX::XMStoreFloat3(&right, Right);
        DirectX::XMStoreFloat3(&up, Up);

        // マウスの移動距離算出
        float moveX = _INPUT_VALUE("MouseMoveX") * _movePower;
        float moveY = _INPUT_VALUE("MouseMoveY") * _movePower;
        if (::GetAsyncKeyState(VK_RBUTTON) & 0x8000)
        {
            // Y軸回転
            _angle.y += moveX * 0.5f;
            if (_angle.y > DirectX::XM_PI)
                _angle.y -= DirectX::XM_2PI;
            else if (_angle.y < -DirectX::XM_PI)
                _angle.y += DirectX::XM_2PI;
            // X軸回転
            _angle.x += moveY * 0.5f;
            if (_angle.x > DirectX::XMConvertToRadians(89.9f))
                _angle.x = DirectX::XMConvertToRadians(89.9f);
            else if (_angle.x < -DirectX::XMConvertToRadians(89.9f))
                _angle.x = -DirectX::XMConvertToRadians(89.9f);
        }
        else if (::GetAsyncKeyState(VK_MBUTTON) & 0x8000)
        {
            // 視点移動
            _eye -= right * moveX * _targetMovePower;
            _eye += up * moveY * _targetMovePower;
        }
        {
            // ズーム
            _eye += _zoomPower * front * _INPUT_VALUE("MouseOldWheel") / 60.0f;
        }

        // 視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
        _target.x = _eye.x + front.x * _range;
        _target.y = _eye.y + front.y * _range;
        _target.z = _eye.z + front.z * _range;

        // カメラの視点と注意点を設定
        mainCamera->SetLookAt(_eye, _target, DirectX::XMFLOAT3(0, 1, 0));
#else
        // カメラ回転値を回転行列に変換
        DirectX::XMMATRIX Transform =
            DirectX::XMMatrixRotationRollPitchYaw(angle_.x, angle_.y, angle_.z);

        // 回転行列から右方向、上方向、前方向ベクトルを取り出す
        DirectX::XMVECTOR Right = Transform.r[0];
        DirectX::XMVECTOR Up = Transform.r[1];
        DirectX::XMVECTOR Front = Transform.r[2];
        Vector3 front{}, right{}, up{};
        DirectX::XMStoreFloat3(&front, Front);
        DirectX::XMStoreFloat3(&right, Right);
        DirectX::XMStoreFloat3(&up, Up);

        // マウスの移動距離算出
        float moveX = INPUT_IS_MOVED("MouseMoveX") * movePower_;
        float moveY = INPUT_IS_MOVED("MouseMoveY") * movePower_;
        if (::GetAsyncKeyState(VK_RBUTTON) & 0x8000)
        {
            // Y軸回転
            angle_.y += moveX * 0.5f;
            if (angle_.y > DirectX::XM_PI)
                angle_.y -= DirectX::XM_2PI;
            else if (angle_.y < -DirectX::XM_PI)
                angle_.y += DirectX::XM_2PI;
            // X軸回転
            angle_.x += moveY * 0.5f;
            if (angle_.x > DirectX::XMConvertToRadians(89.9f))
                angle_.x = DirectX::XMConvertToRadians(89.9f);
            else if (angle_.x < -DirectX::XMConvertToRadians(89.9f))
                angle_.x = -DirectX::XMConvertToRadians(89.9f);
        }
        else if (::GetAsyncKeyState(VK_MBUTTON) & 0x8000)
        {
            // ターゲット移動
            target_ -= right * moveX * targetMovePower_;
            target_ += up * moveY * targetMovePower_;
        }
        {
            // ズーム

            target_ += zoomPower_ * front * INPUT_IS_MOVED("MouseOldWheel") / 60.0f;
        }

        // 注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
        eye_.x = target_.x - front.x * range_;
        eye_.y = target_.y - front.y * range_;
        eye_.z = target_.z - front.z * range_;

        // カメラの視点と注意点を設定
        Camera::Instance().SetLookAt(eye_, target_, DirectX::XMFLOAT3(0, 1, 0));
#endif // _ROTATION_EYE
    }
    else
    {
        // 使用フラグをオフ
        _isActive = false;
    }
}

// デバッグ用Gui
void DebugCamera::DrawGui()
{
    ImGui::DragFloat("movePower", &_movePower, 0.01f);
    ImGui::DragFloat("targetMovePower", &_targetMovePower, 0.01f);
    ImGui::DragFloat("zoomPower", &_zoomPower, 0.01f);
    CameraControllerBase::DrawGui();
}
