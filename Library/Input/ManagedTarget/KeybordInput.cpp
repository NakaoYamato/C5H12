#include "KeybordInput.h"
#include <string>

/// 入力情報更新
void KeybordInputObserver::Update()
{
	// 登録しているすべてのキー入力を確認
	for (auto& keystate : keystates)
	{
		keystate.second = GetAsyncKeyState(keystate.first) & 0x8000 ? TRUE : FALSE;
	}

	// 入力量がある情報の更新
	keyparameters[KEYBORD_AXIS_LX] = static_cast<float>(-keystates['A'] + keystates['D']);
	keyparameters[KEYBORD_AXIS_LY] = static_cast<float>(-keystates['S'] + keystates['W']);
	keyparameters[KEYBORD_AXIS_RX] = static_cast<float>(-keystates[VK_LEFT] + keystates[VK_RIGHT]);
	keyparameters[KEYBORD_AXIS_RY] = static_cast<float>(-keystates[VK_DOWN] + keystates[VK_UP]);
}

/// キーボードのキー番号から文字列に変換
const char* KeybordInputObserver::ToString(int vKey)
{
	switch (vKey)
	{
	case VK_BACK:	return u8"Backspace";
	case VK_TAB:	return u8"Tab";
	case VK_CLEAR:	return u8"Clear";
	case VK_RETURN:	return u8"Enter";
	case VK_SHIFT:	return u8"Shift";
	case VK_CONTROL:	return u8"Ctrl";
	case VK_MENU:	return u8"ALT";

	case VK_LEFT:	return u8"左方向";
	case VK_UP:		return u8"上方向";
	case VK_RIGHT:	return u8"右方向";
	case VK_DOWN:	return u8"下方向";

	case '0':	return u8"0";
	case '1':	return u8"1";
	case '2':	return u8"2";
	case '3':	return u8"3";
	case '4':	return u8"4";
	case '5':	return u8"5";
	case '6':	return u8"6";
	case '7':	return u8"7";
	case '8':	return u8"8";
	case '9':	return u8"9";

	case 'A':	return u8"A";
	case 'B':	return u8"B";
	case 'C':	return u8"C";
	case 'D':	return u8"D";
	case 'E':	return u8"E";
	case 'F':	return u8"F";
	case 'G':	return u8"G";
	case 'H':	return u8"H";
	case 'I':	return u8"I";
	case 'J':	return u8"J";
	case 'K':	return u8"K";
	case 'L':	return u8"L";
	case 'M':	return u8"M";
	case 'N':	return u8"N";
	case 'O':	return u8"O";
	case 'P':	return u8"P";
	case 'Q':	return u8"Q";
	case 'R':	return u8"R";
	case 'S':	return u8"S";
	case 'T':	return u8"T";
	case 'U':	return u8"U";
	case 'V':	return u8"V";
	case 'W':	return u8"W";
	case 'X':	return u8"X";
	case 'Y':	return u8"Y";
	case 'Z':	return u8"Z";
	}
	return " ";
}
