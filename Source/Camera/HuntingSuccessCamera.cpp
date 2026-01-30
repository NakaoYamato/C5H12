#include "HuntingSuccessCamera.h"

#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// 開始時処理
void HuntingSuccessCamera::OnStart()
{
    // スキップ可能
	SetCanSkip(true);
}

// 更新時処理
void HuntingSuccessCamera::OnUpdate(float elapsedTime)
{
    Vector3 focus{}, eye{};

    _timer += elapsedTime;

    // 注視点計算
    focus = _targetPosition;
    focus.y += _heightOffset;
    switch (_state)
    {
    case State::HorizontalMove:
    {
        float rate = MathF::Clamp01(_timer / _horizontalTime);
        float angle = MathF::Lerp(
            _horizontalStartAngle,
            _horizontalEndAngle,
            rate);

        // カメラ位置計算
        eye = focus;
        float sin = std::sin(angle) * _distance;
        float cos = std::cos(angle) * _distance;
        eye.y += _eyeOffset;
        eye.x += _targetForward.x * cos - _targetForward.z * sin;
        eye.z += _targetForward.z * cos + _targetForward.x * sin;

        if (rate >= 1.0f)
        {
            _state = State::Wait;
            _timer = 0.0f;
        }

        break;
    }
    case State::Wait:
    {
        float rate = MathF::Clamp01(_timer / _horizontalTime);
        eye = focus + _targetForward * _distance;
        eye.y += _eyeOffset;
        eye += eye * rate * _waitDistanceRate;

        if (rate >= 1.0f)
        {
            _state = State::HorizontalMove;
            _timer = 0.0f;
            // 切り替え
			if (!_nextControllerName.empty())
			{
				GetActor()->GetScene()->GetMainCameraActor()->SwitchController(_nextControllerName);
				_nextControllerName.clear();
				break;
			}
            else
            {
                GetActor()->GetScene()->GetMainCameraActor()->SwitchPreviousController();
                break;
            }
            GetActor()->GetScene()->GetMainCameraActor()->SwitchPreviousController();
        }
        break;
    }
    }

    GetActor()->GetTransform().SetPosition(eye);
    GetActor()->GetScene()->GetMainCamera()->SetLookAt(eye, focus, Vector3::Up);
}

// GUI描画
void HuntingSuccessCamera::DrawGui()
{
	CameraControllerBase::DrawGui();
    ImGui::Separator();
    ImGui::DragFloat(u8"カメラ高さオフセット", &_heightOffset, 0.1f, 0.0f, 10.0f);
    ImGui::DragFloat(u8"カメラ視線オフセット", &_eyeOffset, 0.1f, 0.0f, 10.0f);
    ImGui::DragFloat(u8"カメラ距離", &_distance, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"横移動開始角度", &_horizontalStartAngle, 0.1f, -DirectX::XMConvertToRadians(90.0f), DirectX::XMConvertToRadians(90.0f));
    ImGui::DragFloat(u8"横移動終了角度", &_horizontalEndAngle, 0.1f, -DirectX::XMConvertToRadians(90.0f), DirectX::XMConvertToRadians(90.0f));
    ImGui::DragFloat(u8"横移動時間", &_horizontalTime, 0.1f, 0.1f, 10.0f);
    ImGui::DragFloat(u8"待機時間", &_waitTime, 0.1f, 0.1f, 10.0f);
    ImGui::DragFloat(u8"待機距離倍率", &_waitDistanceRate, 0.01f, 0.0f, 1.0f);

    if (ImGui::Button("Switch to this controller"))
    {
        Swich();
        SetTarget(
            Vector3::Up,
            Vector3::Front,
            10.0f
        );
    }
}

void HuntingSuccessCamera::SetTarget(const Vector3& position, const Vector3& forward, float distance)
{
    _state = State::HorizontalMove;
    _timer = 0.0f;
    _targetPosition = position;
    _targetForward = forward.Normalize();
    _distance = distance;
}
