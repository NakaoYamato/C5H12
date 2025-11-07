#include "InputManager.h"

#include "../../Library/Scene/Scene.h"

#include "Input/ChestInput.h"

#include <imgui.h>

// 生成時処理
void InputManager::OnCreate()
{
	// チェスト用入力コントローラー登録
	auto chestInputController = GetScene()->RegisterActor<Actor>("ChestInput", ActorTag::System);
	chestInputController->SetParent(this);
	auto chestInput = chestInputController->AddComponent<ChestInput>();
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
				controller->OnExecute();
			}
			else
			{
				controller->SetActive(false);
			}
		}
		_nextInputControllerName.clear();
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
		_inputControllers[_currentInputControllerName]->OnEnd();
		_inputControllers[_currentInputControllerName]->SetActive(false);
	}

	_currentInputControllerName = nextInputName;
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
