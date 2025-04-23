#include "Input.h"

#include <math.h>
#include <Xinput.h>
#include <cassert>
#include <imgui.h>

Input::Input()
{
	// アクションの登録
	_inputActionMap["OK"] = { {InputType::InputKeyboard, 'A'}, {InputType::InputGamepad,XINPUT_GAMEPAD_LEFT_SHOULDER},{InputType::InputMouse,VK_LBUTTON}};
	_inputActionMap["Dash"] = { {InputType::InputKeyboard, VK_SHIFT}, {InputType::InputGamepad,XINPUT_GAMEPAD_LEFT_THUMB}};
	_inputActionMap["Avoidance"] = { {InputType::InputKeyboard, VK_SPACE}, {InputType::InputGamepad,XINPUT_GAMEPAD_B}};
	_inputActionMap["Attack1"] = { {InputType::InputGamepad,XINPUT_GAMEPAD_A}, {InputType::InputMouse,VK_LBUTTON} };
	_inputActionMap["Attack2"] = { {InputType::InputGamepad,XINPUT_GAMEPAD_X}, {InputType::InputMouse,VK_RBUTTON} };
	_inputActionMap["LookOn"] = { {InputType::InputGamepad,XINPUT_GAMEPAD_RIGHT_THUMB}, {InputType::InputMouse,VK_MBUTTON} };

	_moveActionMap["AxisLX"] = { {InputType::InputKeyboard,KEYBORD_AXIS_LX} ,{InputType::InputGamepad,GAMEPAD_AXIS_LX} };
	_moveActionMap["AxisLY"] = { {InputType::InputKeyboard,KEYBORD_AXIS_LY} ,{InputType::InputGamepad,GAMEPAD_AXIS_LY} };
	_moveActionMap["AxisRX"] = { {InputType::InputKeyboard,KEYBORD_AXIS_RX} ,{InputType::InputGamepad,GAMEPAD_AXIS_RX} ,{InputType::InputMouse,MOUSE_AXIS_RX} };
	_moveActionMap["AxisRY"] = { {InputType::InputKeyboard,KEYBORD_AXIS_RY} ,{InputType::InputGamepad,GAMEPAD_AXIS_RY} ,{InputType::InputMouse,MOUSE_AXIS_RY} };
	_moveActionMap["MouseMoveX"] = { {InputType::InputMouse,MOUSE_MOVE_X} };
	_moveActionMap["MouseMoveY"] = { {InputType::InputMouse,MOUSE_MOVE_Y} };
	_moveActionMap["MouseWheel"] = { {InputType::InputMouse,MOUSE_WHEEL} };
	_moveActionMap["MouseOldWheel"] = { {InputType::InputMouse,MOUSE_OLD_WHEEL} };

	// 登録したアクションの入力監視情報を格納
	for (const auto& mapInfo : _inputActionMap)
	{
		_lastInput[mapInfo.first] = FALSE;
		_currentInput[mapInfo.first] = FALSE;
	}
}

// インプット生成
// プログラム開始時に呼び出す
void Input::Initialize(HWND hwnd)
{
	_mouseInputObserver = std::make_unique<MouseInputObserver>(hwnd);
	this->_hwnd = hwnd;
}

/// 更新処理
void Input::Update()
{
	_lastInput = _currentInput;

	// 入力監視クラスを更新
	_keybordInputObserver.Update();
	_gamePadInputObserver.Update();
	_mouseInputObserver->Update();

	// 入力情報取得
	const std::unordered_map<int, BOOL>* inputStates[3] =
	{
		&_keybordInputObserver.keystates,
		&_gamePadInputObserver.gamepadstates,
		&_mouseInputObserver->mousestates
	};

	//それぞれのアクション名に割り当たっている全ての入力をチェック
	for (const auto& mapInfo : _inputActionMap)
	{
		BOOL isPressed = FALSE;
		for (const auto& inputInfo : mapInfo.second)
		{
			const std::unordered_map<int, BOOL>* inputState = inputStates[static_cast<int>(inputInfo.type)];
			// 要素があるかチェック
			assert(inputState->find(inputInfo.buttonID) != inputState->end());
			isPressed = inputState->at(inputInfo.buttonID);

			if (isPressed == TRUE)
				break;
		}

		_currentInput[mapInfo.first] = isPressed;
	}

	// 入力量があるアクションの更新
	const std::unordered_map<int, float>* movedParameters[3] =
	{
		&_keybordInputObserver.keyparameters,
		&_gamePadInputObserver.gamepadParameters,
		&_mouseInputObserver->mouseParameters,
	};

	//それぞれのアクション名に割り当たっている全ての入力をチェック
	for (const auto& mapInfo : _moveActionMap)
	{
		float moved = 0.0f;
		for (const auto& movedInfo : mapInfo.second)
		{
			const std::unordered_map<int, float>* movedParameter = movedParameters[static_cast<int>(movedInfo.type)];
			// 要素があるかチェック
			assert(movedParameter->find(movedInfo.buttonID) != movedParameter->end());
			moved = movedParameter->at(movedInfo.buttonID);

			if (moved != 0.0f)
				break;
		}

		_currentMovedParameter[mapInfo.first] = moved;
	}
}

/// デバッグGUI表示
void Input::DrawGui()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu(u8"デバッグ"))
		{
			ImGui::Checkbox(u8"入力情報", &_showGui);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (_showGui)
	{
		if (ImGui::Begin(u8"入力情報"))
		{
			if (ImGui::TreeNode(u8"押下情報"))
			{
				for (auto& [str, flag] : _currentInput)
				{
					bool v = flag == TRUE;
					ImGui::Checkbox(str.c_str(), &v);
					if (ImGui::TreeNode(u8"入力対象"))
					{
						auto iter = _inputActionMap.find(str);
						for (auto& inputMapInfo : (*iter).second)
						{
							switch (inputMapInfo.type)
							{
							case InputType::InputKeyboard:
								ImGui::Text(u8"キーボード:%s", KeybordInputObserver::ToString(inputMapInfo.buttonID));
								break;
							case InputType::InputGamepad:
								ImGui::Text(u8"ゲームパッド:%s", GamePadInputObserver::ToString(inputMapInfo.buttonID));
								break;
							case InputType::InputMouse:
								ImGui::Text(u8"マウス:%s", MouseInputObserver::ToString(inputMapInfo.buttonID));
								break;
							}
						}

						ImGui::TreePop();
					}
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode(u8"入力値"))
			{
				for (auto& [str, parameter] : _currentMovedParameter)
				{
					ImGui::Text(u8"%s:%f", str.c_str(), parameter);
				}
				ImGui::TreePop();
			}
		}
		ImGui::End();
	}
}

/// 押されているか確認
bool Input::IsPressed(const std::string& action) const
{
	auto it = _currentInput.find(action);

	// 要素があるかチェック
	assert(it != _currentInput.end());

	return it->second;
}

/// 押された瞬間か確認
bool Input::IsTriggerd(const std::string& action) const
{
	// 押されていて
	if (IsPressed(action))
	{
		auto it = _lastInput.find(action);

		// 直前に押されていたらfalse
		// 押されていなかったらtrueを返す
		return !it->second;
	}

	return false;
}

/// 離した瞬間か確認
bool Input::IsReleased(const std::string& action) const
{
	// 押されていていなくて
	if (!IsPressed(action))
	{
		auto it = _lastInput.find(action);

		// 直前に押されていたらtrue
		// 押されていなかったらfalseを返す
		return it->second;
	}

	return false;
}

/// 入力量があるアクションの値取得
float Input::IsAxis(const std::string& action) const
{
	auto it = _currentMovedParameter.find(action);

	// 要素があるかチェック
	assert(it != _currentMovedParameter.end());

	return it->second;
}

void Input::ClearMapData()
{
	for (const auto& mapInfo : _inputActionMap)
	{
		_lastInput[mapInfo.first] = FALSE;
		_currentInput[mapInfo.first] = FALSE;
	}
	for (const auto& mapInfo : _currentMovedParameter)
	{
		_currentMovedParameter[mapInfo.first] = 0.0f;
	}
}
