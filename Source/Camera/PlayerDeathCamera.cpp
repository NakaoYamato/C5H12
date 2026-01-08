#include "PlayerDeathCamera.h"

#include <imgui.h>

// 起動時関数
void PlayerDeathCamera::OnActivate()
{
	// プレイヤー情報設定されていなければエラー通知
	assert(_playerActor);

	_timer = 0.0f;
}

// 更新時処理
void PlayerDeathCamera::OnUpdate(float elapsedTime)
{
	if (!_playerActor)
	{
		return;
	}

	float rate = MathF::Clamp01(_timer / _duration);
	_timer += elapsedTime;

	Vector3 focus{}, eye{};
	// 注視点計算
	focus = _playerActor->GetTransform().GetPosition();
	focus.y += _focusVerticalOffset;

	// カメラ位置計算
	// プレイヤーを前から見る
	eye = focus;
	eye += _playerActor->GetTransform().GetAxisZ() * _cameraDistance;
	eye.y += _cameraUp + _cameraUpRate * rate;
	GetActor()->GetScene()->GetMainCamera()->SetLookAt(eye, focus, Vector3::Up);
	GetActor()->GetTransform().SetPosition(eye);

	// 一定時間経過したら次のカメラへ切り替え
	if (_timer >= _duration)
	{
		GetActor()->GetScene()->GetMainCameraActor()->SwitchController(_nextControllerName);
	}
}

// GUI描画
void PlayerDeathCamera::DrawGui()
{
	ImGui::DragFloat(u8"注視点垂直オフセット", &_focusVerticalOffset, 0.1f, -10.0f, 10.0f);
	ImGui::DragFloat(u8"カメラ距離", &_cameraDistance, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat(u8"カメラ上方向オフセット", &_cameraUp, 0.1f, -10.0f, 10.0f);
	ImGui::DragFloat(u8"カメラ上方向オフセット変化量", &_cameraUpRate, 0.1f, -10.0f, 10.0f);
}

// プレイヤーアクター設定
void PlayerDeathCamera::SetPlayerActor(PlayerActor* playerActor)
{
	_playerActor = playerActor;
}
