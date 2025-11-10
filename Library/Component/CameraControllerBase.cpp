#include "CameraControllerBase.h"

#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// 生成時処理
void CameraControllerBase::OnCreate()
{
	GetActor()->GetScene()->GetMainCameraActor()->RegisterCameraController(this);
	// 親設定
	GetActor()->SetParent(GetActor()->GetScene()->GetMainCameraActor().get());
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
