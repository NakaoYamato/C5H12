#include "Input.h"

#include <math.h>
#include <cassert>
#include <imgui.h>

Input::Input()
{
#pragma region アクションの登録
	_buttonActionMap["OK"] = { {InputType::Keyboard, 'A'}, {InputType::XboxPad,XINPUT_GAMEPAD_LEFT_SHOULDER},{InputType::Mouse,VK_LBUTTON} };
	_buttonActionMap["Dash"] = { {InputType::Keyboard, VK_SHIFT}, {InputType::XboxPad,XINPUT_GAMEPAD_RIGHT_SHOULDER}, {InputType::DirectPad, DIRECTPAD_BUTTON_R1} };
	_buttonActionMap["Evade"] = { {InputType::Keyboard, VK_SPACE}, {InputType::XboxPad,XINPUT_GAMEPAD_A}, {InputType::DirectPad, DIRECTPAD_BUTTON_R_DOWN} };
	_buttonActionMap["Action1"] = { {InputType::XboxPad,XINPUT_GAMEPAD_Y}, {InputType::Mouse,VK_LBUTTON}, {InputType::DirectPad, DIRECTPAD_BUTTON_R_UP} };
	_buttonActionMap["Action2"] = { {InputType::XboxPad,XINPUT_GAMEPAD_B}, {InputType::Mouse,VK_RBUTTON}, {InputType::DirectPad, DIRECTPAD_BUTTON_R_RIGHT} };
	_buttonActionMap["Guard"] = { {InputType::Mouse,VK_XBUTTON2}, {InputType::DirectPad, DIRECTPAD_BUTTON_R2} };
	_buttonActionMap["Use"] = { {InputType::Keyboard, 'E'}, {InputType::XboxPad, XINPUT_GAMEPAD_X}, {InputType::DirectPad, DIRECTPAD_BUTTON_R_LEFT} };
	_buttonActionMap["Collect"] = { {InputType::Keyboard, 'F'}, {InputType::XboxPad, XINPUT_GAMEPAD_B}, {InputType::DirectPad, DIRECTPAD_BUTTON_R_RIGHT} };
	_buttonActionMap["ItemSelect"] = { {InputType::Keyboard, VK_CONTROL}, {InputType::XboxPad,XINPUT_GAMEPAD_LEFT_SHOULDER}, {InputType::DirectPad, DIRECTPAD_BUTTON_L1} };
	_buttonActionMap["Menu"] = { {InputType::Keyboard, VK_ESCAPE}, {InputType::XboxPad,XINPUT_GAMEPAD_BACK}, {InputType::DirectPad, DIRECTPAD_BUTTON_BACK} };
	_buttonActionMap["Start"] = { {InputType::Keyboard, VK_TAB}, {InputType::XboxPad,XINPUT_GAMEPAD_START}, {InputType::DirectPad, DIRECTPAD_BUTTON_START} };
	
	_buttonActionMap["1"] = { {InputType::Keyboard, '1'} };
	_buttonActionMap["2"] = { {InputType::Keyboard, '2'} };
	_buttonActionMap["3"] = { {InputType::Keyboard, '3'} };

	_buttonActionMap["ItemPrevSlide"] = { {InputType::XboxPad,XINPUT_GAMEPAD_X}, {InputType::DirectPad, DIRECTPAD_BUTTON_R_LEFT} };
	_buttonActionMap["ItemNextSlide"] = { {InputType::XboxPad,XINPUT_GAMEPAD_B}, {InputType::DirectPad, DIRECTPAD_BUTTON_R_RIGHT} };

	_buttonActionMap["Up"] = { {InputType::Keyboard, 'W'}, {InputType::Keyboard, VK_UP}, {InputType::XboxPad,XINPUT_GAMEPAD_DPAD_UP}, {InputType::DirectPad, DIRECTPAD_BUTTON_L_UP} };
	_buttonActionMap["Down"] = { {InputType::Keyboard, 'S'}, {InputType::Keyboard, VK_DOWN}, {InputType::XboxPad,XINPUT_GAMEPAD_DPAD_DOWN}, {InputType::DirectPad, DIRECTPAD_BUTTON_L_DOWN} };
	_buttonActionMap["Left"] = { {InputType::Keyboard, 'A'}, {InputType::Keyboard, VK_LEFT}, {InputType::XboxPad,XINPUT_GAMEPAD_DPAD_LEFT}, {InputType::DirectPad, DIRECTPAD_BUTTON_L_LEFT} };
	_buttonActionMap["Right"] = { {InputType::Keyboard, 'D'}, {InputType::Keyboard, VK_RIGHT}, {InputType::XboxPad,XINPUT_GAMEPAD_DPAD_RIGHT}, {InputType::DirectPad, DIRECTPAD_BUTTON_L_RIGHT} };
	_buttonActionMap["Select"] = { {InputType::Keyboard, 'F'}, {InputType::XboxPad,XINPUT_GAMEPAD_A}, {InputType::DirectPad, DIRECTPAD_BUTTON_R_DOWN} };
	_buttonActionMap["Back"] = { {InputType::Keyboard, VK_ESCAPE}, {InputType::Keyboard, VK_BACK}, {InputType::XboxPad,XINPUT_GAMEPAD_B}, {InputType::DirectPad, DIRECTPAD_BUTTON_R_RIGHT} };
	_buttonActionMap["L3"] = { {InputType::Keyboard, VK_SHIFT}, {InputType::XboxPad,XINPUT_GAMEPAD_LEFT_THUMB}, {InputType::DirectPad, DIRECTPAD_BUTTON_L3}};
	_buttonActionMap["R3"] = { {InputType::Keyboard, 'Q'}, {InputType::XboxPad,XINPUT_GAMEPAD_RIGHT_THUMB}, {InputType::DirectPad, DIRECTPAD_BUTTON_R3}};

	_valueActionMap["AxisLX"] = { {InputType::Keyboard,KEYBORD_AXIS_LX}, {InputType::XboxPad,XBOXPAD_AXIS_LX}, {InputType::DirectPad, DIRECTPAD_AXIS_LX} };
	_valueActionMap["AxisLY"] = { {InputType::Keyboard,KEYBORD_AXIS_LY}, {InputType::XboxPad,XBOXPAD_AXIS_LY}, {InputType::DirectPad, DIRECTPAD_AXIS_LY} };
	_valueActionMap["AxisRX"] = { {InputType::Keyboard,KEYBORD_AXIS_RX}, {InputType::XboxPad,XBOXPAD_AXIS_RX}, {InputType::Mouse,MOUSE_AXIS_RX}, {InputType::DirectPad, DIRECTPAD_AXIS_RX} };
	_valueActionMap["AxisRY"] = { {InputType::Keyboard,KEYBORD_AXIS_RY}, {InputType::XboxPad,XBOXPAD_AXIS_RY}, {InputType::Mouse,MOUSE_AXIS_RY}, {InputType::DirectPad, DIRECTPAD_AXIS_RY} };
	_valueActionMap["MouseMoveX"] = { {InputType::Mouse,MOUSE_MOVE_X} };
	_valueActionMap["MouseMoveY"] = { {InputType::Mouse,MOUSE_MOVE_Y} };
	_valueActionMap["MouseWheel"] = { {InputType::Mouse,MOUSE_WHEEL} };
	_valueActionMap["MouseOldWheel"] = { {InputType::Mouse,MOUSE_OLD_WHEEL} };
	_valueActionMap["Guard"] = { {InputType::XboxPad,XBOXPAD_TRIGGER_R} };

	_buttonActionMap["LeftClick"] = { {InputType::Mouse,VK_LBUTTON} };
	_buttonActionMap["RightClick"] = { {InputType::Mouse,VK_RBUTTON} };
	_buttonActionMap["MiddleClick"] = { {InputType::Mouse,VK_MBUTTON} };
#pragma endregion

	// 登録したアクションの入力監視情報を格納
	for (const auto& mapInfo : _buttonActionMap)
	{
		_lastInput[mapInfo.first] = FALSE;
		_currentInput[mapInfo.first] = FALSE;
	}
}

// インプット生成
// プログラム開始時に呼び出す
void Input::Initialize(HWND hwnd, HINSTANCE instance)
{
	_mouseInput = std::make_unique<MouseInput>(hwnd);
	_directInput = std::make_unique<DirectInput>(hwnd, instance);
	this->_hwnd = hwnd;
}

/// 更新処理
void Input::Update()
{
	// 直前の入力情報を保存
	_lastInput = _currentInput;

	// ウィンドウがアクティブかどうかを確認
	if (GetForegroundWindow() != _hwnd)
    {      
		// ウィンドウがアクティブでない場合、入力を無効にする
        for (auto& mapInfo : _buttonActionMap)
        {
            _currentInput[mapInfo.first] = FALSE;
        }
        for (auto& mapInfo : _valueActionMap)
        {
            _currentMovedParameter[mapInfo.first] = 0.0f;
        }
        return;
	}

	// 入力監視クラスを更新
	_keybordInput.Update();
	_gamePadInput.Update();
	_mouseInput->Update();
	_directInput->Update();

	// 入力情報取得
	const std::unordered_map<int, BOOL>* inputStates[] =
	{
		_keybordInput.GetStates(),
		_mouseInput->GetStates(),
		_gamePadInput.GetStates(),
		_directInput->GetStates(),
	};

	//それぞれのアクション名に割り当たっている全ての入力をチェック
	for (const auto& mapInfo : _buttonActionMap)
	{
		BOOL isPressed = FALSE;
		for (const auto& inputInfo : mapInfo.second)
		{
			const std::unordered_map<int, BOOL>* inputState = inputStates[static_cast<int>(inputInfo.type)];
			// 要素があるかチェック
			assert(inputState->find(inputInfo.buttonID) != inputState->end());

			isPressed = inputState->at(inputInfo.buttonID);

			if (isPressed == TRUE)
			{
				// どのデバイスが最後に入力されたかを保存
				_currentInputDevice = inputInfo.type;
				break;
			}
		}

		_currentInput[mapInfo.first] = isPressed;
	}

	// 入力量があるアクションの更新
	const std::unordered_map<int, float>* movedParameters[] =
	{
		_keybordInput.GetValues(),
		_mouseInput->GetValues(),
		_gamePadInput.GetValues(),
		_directInput->GetValues(),
	};

	//それぞれのアクション名に割り当たっている全ての入力をチェック
	for (const auto& mapInfo : _valueActionMap)
	{
		float moved = 0.0f;
		for (const auto& movedInfo : mapInfo.second)
		{
			const std::unordered_map<int, float>* movedParameter = movedParameters[static_cast<int>(movedInfo.type)];
			// 要素があるかチェック
			assert(movedParameter->find(movedInfo.buttonID) != movedParameter->end());
			moved = movedParameter->at(movedInfo.buttonID);

			if (moved != 0.0f)
			{
				// どのデバイスが最後に入力されたかを保存
				_currentInputDevice = movedInfo.type;
				break;
			}
		}

		_currentMovedParameter[mapInfo.first] = moved;
	}

	// 最後に入力されたデバイスがマウスの場合、キーボード扱いにする
	if (_currentInputDevice == InputType::Mouse)
	{
		_currentInputDevice = InputType::Keyboard;
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
			ImGui::Text(u8"現在の入力デバイス:%s",
				_currentInputDevice == InputType::Keyboard ? u8"キーボード" :
				_currentInputDevice == InputType::XboxPad ? u8"ゲームパッド" :
				_currentInputDevice == InputType::Mouse ? u8"マウス" :
				_currentInputDevice == InputType::DirectPad ? u8"ダイレクトパッド" : u8"不明");
			ImGui::Separator();

			if (ImGui::TreeNode(u8"押下情報"))
			{
				for (auto& [str, flag] : _currentInput)
				{
					bool v = flag == TRUE;
					ImGui::Checkbox(str.c_str(), &v);
					if (ImGui::TreeNode(u8"入力対象"))
					{
						auto iter = _buttonActionMap.find(str);
						for (auto& inputMapInfo : (*iter).second)
						{
							switch (inputMapInfo.type)
							{
							case InputType::Keyboard:
								ImGui::Text(u8"キーボード:%s", KeybordInput::ToString(inputMapInfo.buttonID));
								break;
							case InputType::XboxPad:
								ImGui::Text(u8"ゲームパッド:%s", XboxPadInput::ToString(inputMapInfo.buttonID));
								break;
							case InputType::Mouse:
								ImGui::Text(u8"マウス:%s", MouseInput::ToString(inputMapInfo.buttonID));
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

			if (ImGui::TreeNode(u8"振動テスト"))
			{
				if (ImGui::Button(u8"振動開始：左モーター"))
				{
					_gamePadInput.SetVibration(1.0f, 0.0f);
				}
				if (ImGui::Button(u8"振動開始：右モーター"))
				{
					_gamePadInput.SetVibration(0.0f, 1.0f);
				}
				if (ImGui::Button(u8"振動停止"))
				{
					_gamePadInput.SetVibration(0.0f, 0.0f);
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
float Input::IsValue(const std::string& action) const
{
	auto it = _currentMovedParameter.find(action);

	// 要素があるかチェック
	assert(it != _currentMovedParameter.end());

	return it->second;
}

void Input::ClearMapData()
{
	for (const auto& mapInfo : _buttonActionMap)
	{
		_lastInput[mapInfo.first] = FALSE;
		_currentInput[mapInfo.first] = FALSE;
	}
	for (const auto& mapInfo : _currentMovedParameter)
	{
		_currentMovedParameter[mapInfo.first] = 0.0f;
	}
}
