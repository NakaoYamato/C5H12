#include "MainCamera.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Component/CameraControllerBase.h"

#include "../../Source/Camera/PlayerCameraController.h"
#include "../../Source/Camera/HuntingSuccessCamera.h"
#include "../../Source/Camera/LockOnCamera.h"
#include "../../Source/Camera/ChangeArmorCamera.h"
#include "../../Source/Camera/PlayerDeathCamera.h"

#include "../../Scene/Scene.h"

#include <imgui.h>

// 生成時処理
void MainCamera::OnCreate()
{
	// カメラの初期化
	GetScene()->GetMainCamera()->SetLookAt(
		Vector3(0.0f, 10.0f, -10.0f),
		Vector3(0.0f, 0.0f, 0.0f),
		Vector3(0.0f, 1.0f, 0.0f)
	);
    float screenWidth = Graphics::Instance().GetScreenWidth();
    float screenHeight = Graphics::Instance().GetScreenHeight();
	GetScene()->GetMainCamera()->SetPerspectiveFov(
        DirectX::XMConvertToRadians(50),	// 画角
        screenWidth / screenHeight,			// 画面アスペクト比
        0.1f,								// ニアクリップ
        1000.0f								// ファークリップ
	);

	// カメラコンポーネント生成
	this->AddComponent<PlayerCameraController>();
	this->AddComponent<HuntingSuccessCamera>();
	this->AddComponent<LockOnCamera>();
	this->AddComponent<ChangeArmorCamera>();
	this->AddComponent<PlayerDeathCamera>();
}

// 開始時処理
void MainCamera::OnStart()
{
}

// 更新処理
void MainCamera::OnUpdate(float elapsedTime)
{
	// コントローラーの更新
	if (!_nextControllerName.empty())
	{
		for (auto& [name, controller] : _cameraControllers)
		{
			if (name == _nextControllerName)
			{
				controller->SetActive(true);
				controller->OnActivate();
			}
			else
			{
				controller->SetActive(false);
			}
		}
		_nextControllerName.clear();
	}
	
	// シェイクマネージャーの更新
	_shakeManager.Update(elapsedTime);

	_callbackEyeOffset = Vector3::Zero;
	for (auto it = _onUpdateCallbacks.begin(); it != _onUpdateCallbacks.end(); )
	{
		if ((*it)(elapsedTime, this))
		{
			it = _onUpdateCallbacks.erase(it);
		}
		else
		{
			++it;
		}
	}
	
	// シェイクによるオフセットを取得して
	Vector3 shakeOffset = _shakeManager.GetTotalOffset(_transform.GetPosition());
	// 既存の_eyeOffsetに合成
	_callbackEyeOffset += shakeOffset;
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
			ImGui::SliderFloat(u8"カメラ感度倍率", &_cameraSensitivity, 0.01f, 1.0f);
			ImGui::Checkbox(u8"垂直操作反転", &_isInvertY);
			ImGui::Checkbox(u8"水平操作反転", &_isInvertX);

			ImGui::SameLine();
			ImGui::Text(u8"使用中のカメラ:");
			ImGui::SameLine();
			ImGui::Text(_currentControllerName.c_str());

			if (ImGui::TreeNode(u8"変更履歴"))
			{
				int index = 0;
				for (auto& history : _cameraControllerHistory)
				{
					ImGui::Text((std::to_string(index++) + ": " + history.c_str()).c_str());
				}
				ImGui::TreePop();
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(u8"コールバック"))
		{
			if (ImGui::Button("Test1"))
			{
				static float TestTimer = 0.0f;
				std::function<bool(float, MainCamera*)> testCallback =
					[](float elapsedTime, MainCamera* receiver) -> bool
					{
						receiver->AddEyeOffset(Vector3(0.0f, 0.0f, -5.0f) * TestTimer);

						TestTimer += elapsedTime;
						if (TestTimer > 1.0f)
						{
							TestTimer = 0.0f;
							return true;
						}
						return false;
					};
				this->AddOnUpdateCallback(testCallback);
			}
			if (ImGui::Button("Test2"))
			{
				static float TestTimer = 0.0f;
				std::function<bool(float, MainCamera*)> testCallback =
					[](float elapsedTime, MainCamera* receiver) -> bool
					{
						receiver->AddEyeOffset(Vector3(0.0f, 0.0f, -5.0f));
						return true;
					};
				this->AddOnUpdateCallback(testCallback);
			}
			ImGui::Text(u8"設定されている関数:%d", _onUpdateCallbacks.size());
			ImGui::DragFloat3(u8"目のオフセット", &_callbackEyeOffset.x, 0.1f);

			_shakeManager.DrawGui();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

// コントローラーの登録
void MainCamera::RegisterController(CameraControllerRef controller)
{
	_cameraControllers[controller->GetName()] = controller;
}

// 切り替え(直前のコントローラーに戻す)
void MainCamera::SwitchPreviousController()
{
	if (_cameraControllers.size() == 0)
		return;
	std::string lastInput = _cameraControllerHistory.back();
	SwitchController(lastInput);
}

// 切り替え
void MainCamera::SwitchController(const std::string& nextControllerName)
{
	// 履歴に追加
	if (!_currentControllerName.empty())
		_cameraControllerHistory.push_back(_currentControllerName);

	_nextControllerName = nextControllerName;
	if (!_currentControllerName.empty() &&
		_cameraControllers[_currentControllerName] != nullptr)
	{
		_cameraControllers[_currentControllerName]->OnDeactivate();
		_cameraControllers[_currentControllerName]->SetActive(false);
	}

	_currentControllerName = nextControllerName;
}

// 更新処理追加
// 戻り値がtrueの場合、登録解除される
void MainCamera::AddOnUpdateCallback(const std::function<bool(float, MainCamera*)>& callback)
{
	_onUpdateCallbacks.push_back(callback);
}
