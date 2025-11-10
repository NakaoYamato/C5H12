#include "MainCamera.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Component/CameraControllerBase.h"

#include "../../Source/Camera/PlayerCameraController.h"

#include "../../Scene/Scene.h"

#include <imgui.h>

// 生成時処理
void MainCamera::OnCreate()
{
	// カメラの初期化
	SetLookAt(
		Vector3(0.0f, 10.0f, -10.0f),
		Vector3(0.0f, 0.0f, 0.0f),
		Vector3(0.0f, 1.0f, 0.0f)
	);
    float screenWidth = Graphics::Instance().GetScreenWidth();
    float screenHeight = Graphics::Instance().GetScreenHeight();
	SetPerspectiveFov(
        DirectX::XMConvertToRadians(50),	// 画角
        screenWidth / screenHeight,			// 画面アスペクト比
        0.1f,								// ニアクリップ
        1000.0f								// ファークリップ
	);
}

// 更新処理
void MainCamera::OnUpdate(float elapsedTime)
{
	if (!_nextCameraControllerName.empty())
	{
		for (auto& [name, controller] : _cameraControllers)
		{
			if (name == _nextCameraControllerName)
			{
				controller->SetActive(true);
				controller->OnEntry();
			}
			else
			{
				controller->SetActive(false);
			}
		}
		_nextCameraControllerName.clear();
	}
}

// GUI描画
void MainCamera::OnDrawGui()
{
	static ImGuiTabBarFlags tab_bar_flags =
		ImGuiTabBarFlags_AutoSelectNewTabs |
		ImGuiTabBarFlags_Reorderable |
		ImGuiTabBarFlags_FittingPolicyResizeDown;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem(u8"カメラマネージャー"))
		{
			int index = 0;
			for (auto& history : _cameraControllerHistory)
			{
				ImGui::Text((std::to_string(index++) + ": " + history.c_str()).c_str());
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void MainCamera::SetLookAt(const Vector3& eye, const Vector3& focus, const Vector3& up)
{
	GetScene()->GetMainCamera()->SetLookAt(eye, focus, up);
    // アクターのトランスフォームに適応
	_transform.SetPosition(eye);
    DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&GetScene()->GetMainCamera()->GetView()));
    DirectX::XMVECTOR S, R, T;
    DirectX::XMMatrixDecompose(&S, &R, &T, World);
    Vector3 s, r, t;
    r = Quaternion::ToRollPitchYaw(R);
    _transform.SetAngle(r);
}

void MainCamera::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
	GetScene()->GetMainCamera()->SetPerspectiveFov(fovY, aspect, nearZ, farZ);
}

// コントローラーの登録
void MainCamera::RegisterCameraController(CameraControllerRef controller)
{
	_cameraControllers[controller->GetName()] = controller;
}

// コントローラーの登録
void MainCamera::SwitchPreviousController()
{
	if (_cameraControllers.size() == 0)
		return;
	std::string lastInput = _cameraControllerHistory.back();
	SwitchController(lastInput);
}

// コントローラーの登録
void MainCamera::SwitchController(const std::string& nextControllerName)
{
	_cameraControllerHistory.push_back(_currentCameraControllerName);
	_nextCameraControllerName = nextControllerName;
	if (!_currentCameraControllerName.empty() &&
		_cameraControllers[_currentCameraControllerName] != nullptr)
	{
		_cameraControllers[_currentCameraControllerName]->OnExit();
		_cameraControllers[_currentCameraControllerName]->SetActive(false);
	}

	_currentCameraControllerName = nextControllerName;
}
