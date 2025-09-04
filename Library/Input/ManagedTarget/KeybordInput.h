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
class KeybordInput
{
public:
	KeybordInput();
	~KeybordInput() {}

	/// <summary>
	/// 入力情報更新
	/// </summary>
	void Update();
	/// <summary>
	/// キーボードのキー番号から文字列に変換
	/// </summary>
	static const char* ToString(int vKey);
	/// <summary>
	/// 入力情報取得
	/// </summary>
	/// <returns></returns>
	const std::unordered_map<int, BOOL>* GetStates() const { return &_states; }
	/// <summary>
	/// 入力量情報取得
	/// </summary>
	/// <returns></returns>
	const std::unordered_map<int, float>* GetValues() const { return &_values; }
private:
	// キーボード入力情報格納
	// 一覧:https://learn.microsoft.com/ja-jp/windows/win32/inputdev/virtual-key-codes
	std::unordered_map<int, BOOL> _states;
	// 入力量がある情報
	std::unordered_map<int, float> _values;

};