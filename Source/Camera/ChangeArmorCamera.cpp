#include "ChangeArmorCamera.h"

#include "../../Library/Scene/Scene.h"

#include <imgui.h>

void ChangeArmorCamera::Start()
{
}

void ChangeArmorCamera::OnUpdate(float elapsedTime)
{
	auto chestActor = _chestActor.lock();
	if (!chestActor)
		return;

	Vector3 chestWorldPos = chestActor->GetTransform().GetWorldPosition();
	Vector3 chestForward = -chestActor->GetTransform().GetAxisZ();

    Vector3 focus = chestWorldPos;
	focus.y += _cameraOffsetY;
    Vector3 eye = focus + chestForward * _cameraLength;

    GetActor()->GetScene()->GetMainCamera()->SetLookAt(eye, focus, Vector3::Up);
}

// Gui•`‰æ
void ChangeArmorCamera::DrawGui()
{
	CameraControllerBase::DrawGui();
	ImGui::Separator();
	ImGui::DragFloat(u8"ƒJƒƒ‰‹——£", &_cameraLength, 0.1f, 0.0f, 100.0f);
}
