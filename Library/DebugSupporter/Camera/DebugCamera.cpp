#include "DebugCamera.h"

#include "../../Library/Camera/Camera.h"
#include "../../Library/Input/Input.h"
#include "../DebugSupporter.h"

#include <imgui.h>

// 更新処理
void DebugCamera::Update(float elapsedTime)
{
    // 使用フラグをオフ
    isActive_ = false;

    // F4を押していたら起動
    if (Debug::Input::IsActive(DebugInput::BTN_F4))
    {
        // 使用フラグをオン
        isActive_ = true;

        // ImGuiのウィンドウを選択していたら処理しない
#ifdef _DEBUG
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AnyWindow) ||
            ImGui::IsWindowFocused(ImGuiFocusedFlags_::ImGuiFocusedFlags_AnyWindow))
            return;
#endif // _DEBUG

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

            target_ += front * INPUT_IS_MOVED("MouseOldWheel") / 60.0f;

            if (range_ <= 1.0f)
            {
                target_ += front * INPUT_IS_MOVED("MouseOldWheel") / 60.0f;
                range_ = INPUT_IS_MOVED("MouseOldWheel") / 60.0f;
            }
        }

        // 注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
        eye_.x = target_.x - front.x * range_;
        eye_.y = target_.y - front.y * range_;
        eye_.z = target_.z - front.z * range_;

        // カメラの視点と注意点を設定
        Camera::Instance().SetLookAt(eye_, target_, DirectX::XMFLOAT3(0, 1, 0));
    }
}

// デバッグ用Gui
void DebugCamera::DrawGui()
{
    CameraControllerBase::DrawGui();
}
