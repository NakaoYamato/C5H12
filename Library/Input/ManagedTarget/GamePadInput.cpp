#include "GamePadInput.h"

#include <Xinput.h>

GamePadInputObserver::GamePadInputObserver()
{
	// 使用するボタンを設定
	gamepadstates[XINPUT_GAMEPAD_DPAD_UP] = FALSE;
	gamepadstates[XINPUT_GAMEPAD_DPAD_RIGHT] = FALSE;
	gamepadstates[XINPUT_GAMEPAD_DPAD_DOWN] = FALSE;
	gamepadstates[XINPUT_GAMEPAD_DPAD_LEFT] = FALSE;

	gamepadstates[XINPUT_GAMEPAD_A] = FALSE;
	gamepadstates[XINPUT_GAMEPAD_B] = FALSE;
	gamepadstates[XINPUT_GAMEPAD_X] = FALSE;
	gamepadstates[XINPUT_GAMEPAD_Y] = FALSE;

	gamepadstates[XINPUT_GAMEPAD_START] = FALSE;
	gamepadstates[XINPUT_GAMEPAD_BACK] = FALSE;

	gamepadstates[XINPUT_GAMEPAD_LEFT_THUMB] = FALSE;		// L3
	gamepadstates[XINPUT_GAMEPAD_RIGHT_THUMB] = FALSE;		//R3
	gamepadstates[XINPUT_GAMEPAD_LEFT_SHOULDER] = FALSE;	// L1
	gamepadstates[XINPUT_GAMEPAD_RIGHT_SHOULDER] = FALSE;	// R1

	gamepadParameters[GAMEPAD_AXIS_LX] = 0.0f;
	gamepadParameters[GAMEPAD_AXIS_LY] = 0.0f;
	gamepadParameters[GAMEPAD_AXIS_RX] = 0.0f;
	gamepadParameters[GAMEPAD_AXIS_RY] = 0.0f;

	gamepadParameters[GAMEPAD_TRIGGER_L] = 0.0f;
	gamepadParameters[GAMEPAD_TRIGGER_R] = 0.0f;
}

/// 入力情報更新
void GamePadInputObserver::Update()
{
	// ボタン情報取得
	XINPUT_STATE xinputState;
	if (XInputGetState(slot, &xinputState) == ERROR_SUCCESS)
	{
		//XINPUT_CAPABILITIES caps;
		//XInputGetCapabilities(m_slot, XINPUT_FLAG_GAMEPAD, &caps);
		XINPUT_GAMEPAD& pad = xinputState.Gamepad;

		// 登録しているすべてのボタン入力を確認
		for (auto& gamepadstate : gamepadstates)
		{
			gamepadstate.second = pad.wButtons & gamepadstate.first ? TRUE : FALSE;
		}

		// デッドゾーン処理
		if ((pad.sThumbLX <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && pad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
			(pad.sThumbLY <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && pad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
		{
			pad.sThumbLX = 0;
			pad.sThumbLY = 0;
		}

		if ((pad.sThumbRX <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && pad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
			(pad.sThumbRY <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && pad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
		{
			pad.sThumbRX = 0;
			pad.sThumbRY = 0;
		}
		// 入力量がある情報の更新
		gamepadParameters[GAMEPAD_AXIS_LX] = static_cast<float>(pad.sThumbLX) / static_cast<float>(0x8000);
		gamepadParameters[GAMEPAD_AXIS_LY] = static_cast<float>(pad.sThumbLY) / static_cast<float>(0x8000);
		gamepadParameters[GAMEPAD_AXIS_RX] = static_cast<float>(pad.sThumbRX) / static_cast<float>(0x8000);
		gamepadParameters[GAMEPAD_AXIS_RY] = static_cast<float>(pad.sThumbRY) / static_cast<float>(0x8000);

		gamepadParameters[GAMEPAD_TRIGGER_L] = static_cast<float>(pad.bLeftTrigger) / 255.0f;
		gamepadParameters[GAMEPAD_TRIGGER_R] = static_cast<float>(pad.bRightTrigger) / 255.0f;
	}
}

/// ゲームパッドのキー番号から文字列に変換
const char* GamePadInputObserver::ToString(int vKey)
{
	switch (vKey)
	{
	case XINPUT_GAMEPAD_DPAD_UP:	return u8"上十字キー";
	case XINPUT_GAMEPAD_DPAD_RIGHT:	return u8"右十字キー";
	case XINPUT_GAMEPAD_DPAD_DOWN:	return u8"下十字キー";
	case XINPUT_GAMEPAD_DPAD_LEFT:	return u8"左十字キー";

	case XINPUT_GAMEPAD_A:	return u8"A";
	case XINPUT_GAMEPAD_B:	return u8"B";
	case XINPUT_GAMEPAD_X:	return u8"X";
	case XINPUT_GAMEPAD_Y:	return u8"Y";

	case XINPUT_GAMEPAD_START:	return u8"START";
	case XINPUT_GAMEPAD_BACK:	return u8"BACK";

	case XINPUT_GAMEPAD_LEFT_THUMB:	return u8"L3";
	case XINPUT_GAMEPAD_RIGHT_THUMB:return u8"R3";
	case XINPUT_GAMEPAD_LEFT_SHOULDER:	return u8"L1";
	case XINPUT_GAMEPAD_RIGHT_SHOULDER:return u8"R1";
	}

	return " ";
}
