#include "CameraControllerBase.h"

#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// 開始処理
void CameraControllerBase::Start()
{
	GetActor()->GetScene()->GetMainCameraActor()->RegisterController(this);

	// 開始時処理
	OnStart();
}

// 更新処理
void CameraControllerBase::LateUpdate(float elapsedTime)
{
	// F4を押していたらデバッグ用カメラ起動中
	if (Debug::Input::IsActive(DebugInput::BTN_F4))
		return;

	if (_isActive)
	{
		OnUpdate(elapsedTime);
	}
}

// Gui描画
void CameraControllerBase::DrawGui()
{
	ImGui::Checkbox("Active", &_isActive);
}

// このコントローラーに切り替える
void CameraControllerBase::Swich()
{
	GetActor()->GetScene()->GetMainCameraActor()->SwitchController(this->GetName());
}
