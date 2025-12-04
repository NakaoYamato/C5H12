#include "InputManager.h"

#include "../../Library/Scene/Scene.h"

#include "Input/ChestInput.h"
#include "Input/Menu/MenuInput.h"

#include <imgui.h>

// 生成時処理
void InputManager::OnCreate()
{
	// チェスト用入力コントローラー登録
	auto chestInputController = GetScene()->RegisterActor<Actor>("ChestInput", ActorTag::System);
	chestInputController->SetParent(this);
	auto chestInput = chestInputController->AddComponent<ChestInput>();

    // メニュー用入力コントローラー登録
    auto menuInputController = GetScene()->RegisterActor<Actor>("MenuInput", ActorTag::System);
    menuInputController->SetParent(this);
    auto menuInput = menuInputController->AddComponent<MenuInput>();
}

// 開始関数
void InputManager::OnStart()
{
	// プレイヤー入力コントローラーをアクティブにする
	if (_inputControllers.size() > 0)
	{
		SwitchInput("PlayerInput");
	}
}

// 更新前処理
void InputManager::OnPreUpdate(float elapsedTime)
{
	if (!_nextInputControllerName.empty())
	{
		for (auto& [name, controller] : _inputControllers)
		{
			if (name == _nextInputControllerName)
			{
				controller->SetActive(true);
				controller->OnEntry();
			}
			else
			{
				controller->SetActive(false);
			}
		}
		_nextInputControllerName.clear();
	}

	// 振動更新
	if (_vibrationData.duration > 0.0f)
	{
		_vibrationData.elapsedTime += elapsedTime;
		float rate = _vibrationData.duration > 0.0f ?
		_vibrationData.elapsedTime / _vibrationData.duration : 1.0f;
		if (_vibrationData.elapsedTime >= _vibrationData.duration)
		{
			_vibrationData.leftMotor = 0.0f;
			_vibrationData.rightMotor = 0.0f;
			_vibrationData.duration = 0.0f;
			_vibrationData.elapsedTime = 0.0f;
		}
		float leftMotor = MathF::Lerp(_vibrationData.leftMotor, 0.0f, rate);
		float rightMotor = MathF::Lerp(_vibrationData.rightMotor, 0.0f, rate);

		// 振動情報を設定
		Input::Instance().GetGamePadInput()->SetVibration(leftMotor, rightMotor);
	}
	else
	{
		// 振動停止
		Input::Instance().GetGamePadInput()->StopVibration();
	}
}

// GUI描画
void InputManager::OnDrawGui()
{
	static ImGuiTabBarFlags tab_bar_flags =
		ImGuiTabBarFlags_AutoSelectNewTabs |
		ImGuiTabBarFlags_Reorderable |
		ImGuiTabBarFlags_FittingPolicyResizeDown;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem(u8"入力マネージャー"))
		{
			int index = 0;
			for (auto& history : _inputControllerHistory)
			{
				ImGui::Text((std::to_string(index++) + ": " + history.c_str()).c_str());
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

// 入力コントローラーの登録
void InputManager::RegisterInputController(InputControllerRef controller)
{
	_inputControllers[controller->GetName()] = controller;
}

// 入力の切り替え(直前の入力コントローラーに戻す)
void InputManager::SwitchPreviousInput()
{
	if (_inputControllerHistory.size() == 0)
		return;
	std::string lastInput = _inputControllerHistory.back();
	SwitchInput(lastInput);
}

// 入力の切り替え
void InputManager::SwitchInput(const std::string& nextInputName)
{
	_inputControllerHistory.push_back(_currentInputControllerName);
	_nextInputControllerName = nextInputName;
	if (!_currentInputControllerName.empty() &&
		_inputControllers[_currentInputControllerName] != nullptr)
	{
		_inputControllers[_currentInputControllerName]->OnExit();
		_inputControllers[_currentInputControllerName]->SetActive(false);
	}

	_currentInputControllerName = nextInputName;
}

// カメラを動かせるかどうか
bool InputManager::CanMoveCamera() const
{
	if (_inputControllers.find(_currentInputControllerName) != _inputControllers.end())
	{
		return _inputControllers.at(_currentInputControllerName)->CanMoveCamera();
	}
	return false;
}

// 振動設定
void InputManager::SetVibration(float leftMotor, float rightMotor, float duration)
{
	_vibrationData.leftMotor = leftMotor;
	_vibrationData.rightMotor = rightMotor;
	_vibrationData.duration = duration;
	_vibrationData.elapsedTime = 0.0f;
}

// 生成時処理
void InputControllerBase::OnCreate()
{
	// InputManagerを探す
	auto manager = GetActor()->GetScene()->GetActorManager().FindByClass<InputManager>(ActorTag::System);
	if (manager != nullptr)
	{
		_inputManager = manager.get();
		_inputManager->RegisterInputController(this);
	}
}

// 更新処理
void InputControllerBase::Update(float elapsedTime)
{
	if (_isActive)
	{
		OnUpdate(elapsedTime);
	}
}

// この入力に切り替える
void InputControllerBase::Swich()
{
	_inputManager->SwitchInput(GetName());
}
