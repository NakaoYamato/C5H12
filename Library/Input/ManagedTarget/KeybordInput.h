#pragma once

#include <Windows.h>
#include <unordered_map>

static constexpr int KEYBORD_AXIS_LX = 0x00;
static constexpr int KEYBORD_AXIS_LY = 0x01;
static constexpr int KEYBORD_AXIS_RX = 0x02;
static constexpr int KEYBORD_AXIS_RY = 0x03;

/// <summary>
/// キーボードの入力監視クラス
/// </summary>
class KeybordInputObserver
{
public:
	KeybordInputObserver()
	{
		// 使用するキーを設定
		keystates[VK_BACK]		= FALSE;
		keystates[VK_TAB]		= FALSE;
		keystates[VK_CLEAR]		= FALSE;
		keystates[VK_RETURN]	= FALSE;
		keystates[VK_SHIFT]		= FALSE;
		keystates[VK_CONTROL]	= FALSE;
		keystates[VK_MENU]		= FALSE;
		keystates[VK_SPACE]		= FALSE;

		keystates[VK_LEFT]	= FALSE;
		keystates[VK_UP]	= FALSE;
		keystates[VK_RIGHT] = FALSE;
		keystates[VK_DOWN]	= FALSE;

		// 数字
		for (int s = '0'; s <= '9'; s++)
			keystates[s] = FALSE;

		// ローマ字
		for (int s = 'A'; s <= 'Z'; s++)
			keystates[s] = FALSE;

		// 左軸移動量
		keyparameters[KEYBORD_AXIS_LX] = 0.0f;
		keyparameters[KEYBORD_AXIS_LY] = 0.0f;
		// 右軸移動量
		keyparameters[KEYBORD_AXIS_RX] = 0.0f;
		keyparameters[KEYBORD_AXIS_RY] = 0.0f;
	}
	~KeybordInputObserver() {}

	/// <summary>
	/// 入力情報更新
	/// </summary>
	void Update();

	// キーボード入力情報格納
	// 一覧:https://learn.microsoft.com/ja-jp/windows/win32/inputdev/virtual-key-codes
	std::unordered_map<int, BOOL> keystates;

	// 入力量がある情報
	std::unordered_map<int, float> keyparameters;

public:
	/// <summary>
	/// キーボードのキー番号から文字列に変換
	/// </summary>
	static const char* ToString(int vKey);
};