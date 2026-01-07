#include "ChangeArmorCamera.h"

#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// 開始時処理
void ChangeArmorCamera::OnStart()
{
}

// 起動時関数
void ChangeArmorCamera::OnActivate()
{
	// 元のカメラ情報を保存
	auto mainCamera = GetActor()->GetScene()->GetMainCamera();
	_previousCameraEye = mainCamera->GetEye();
	_previousCameraFocus = mainCamera->GetFocus();
	_previousCameraAngle = GetActor()->GetTransform().GetAngle();
}

// 終了時処理
void ChangeArmorCamera::OnDeactivate()
{
	// 元のカメラ情報に戻す
	auto mainCamera = GetActor()->GetScene()->GetMainCamera();
	mainCamera->SetLookAt(_previousCameraEye, _previousCameraFocus, Vector3::Up);
	GetActor()->GetTransform().SetAngle(_previousCameraAngle);
}

void ChangeArmorCamera::OnUpdate(float elapsedTime)
{
	auto chestActor = _chestActor.lock();
	if (!chestActor)
		return;

	Vector3 chestWorldPos = chestActor->GetTransform().GetWorldPosition();
	Vector3 chestRight = chestActor->GetTransform().GetAxisX();
	Vector3 chestForward = -chestActor->GetTransform().GetAxisZ();

    Vector3 focus = chestWorldPos;
	focus += chestRight * _cameraOffsetX;
	focus.y += _cameraOffsetY;
    Vector3 eye = focus + chestForward * _cameraLength;

    GetActor()->GetScene()->GetMainCamera()->SetLookAt(eye, focus, Vector3::Up);
}

// Gui描画
void ChangeArmorCamera::DrawGui()
{
	CameraControllerBase::DrawGui();
	ImGui::Separator();
	ImGui::DragFloat(u8"カメラXオフセット", &_cameraOffsetX, 0.1f, -10.0f, 10.0f);
	ImGui::DragFloat(u8"カメラYオフセット", &_cameraOffsetY, 0.1f, 0.0f, 10.0f);
	ImGui::DragFloat(u8"カメラ距離", &_cameraLength, 0.1f, 0.0f, 100.0f);
}
