#include "CameraControllerBase.h"

#include "../../Library/Scene/Scene.h"

// 生成時処理
void CameraControllerBase::OnCreate()
{
	GetActor()->GetScene()->GetMainCameraActor()->RegisterCameraController(this);
}

// 更新処理
void CameraControllerBase::LateUpdate(float elapsedTime)
{
	if (_isActive)
	{
		OnUpdate(elapsedTime);
	}
}

// このコントローラーに切り替える
void CameraControllerBase::Swich()
{
}
