#pragma once

#include <Windows.h>
#include <unordered_map>

static constexpr int GAMEPAD_AXIS_LX = 0x00;
static constexpr int GAMEPAD_AXIS_LY = 0x01;
static constexpr int GAMEPAD_AXIS_RX = 0x02;
static constexpr int GAMEPAD_AXIS_RY = 0x03;
static constexpr int GAMEPAD_TRIGGER_L = 0x04;
static constexpr int GAMEPAD_TRIGGER_R = 0x05;

/// <summary>
/// ゲームパッドの入力監視クラス
/// </summary>
class GamePadInputObserver
{
public:
	/// <summary>
	/// コンストラクタで取得するボタンを設定している
	/// </summary>
	GamePadInputObserver();
	~GamePadInputObserver() {}

	/// <summary>
	/// 入力情報更新
	/// </summary>
	void Update();

	// ゲームパッド入力情報格納
	// 一覧:https://learn.microsoft.com/ja-jp/windows/win32/inputdev/virtual-key-codes
	std::unordered_map<int, BOOL> gamepadstates;

	// 入力量がある情報
	std::unordered_map<int, float> gamepadParameters;

	int					slot = 0;
public:
	/// <summary>
	/// ゲームパッドのキー番号から文字列に変換
	/// </summary>
	static const char* ToString(int vKey);
};