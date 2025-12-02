#pragma once

#include <Windows.h>
#include <unordered_map>
#include <Xinput.h>

#pragma region 入力値を取得する用の定義
static constexpr int XBOXPAD_AXIS_LX = 0x00;
static constexpr int XBOXPAD_AXIS_LY = 0x01;
static constexpr int XBOXPAD_AXIS_RX = 0x02;
static constexpr int XBOXPAD_AXIS_RY = 0x03;
static constexpr int XBOXPAD_TRIGGER_L = 0x04;
static constexpr int XBOXPAD_TRIGGER_R = 0x05;
#pragma endregion

/// <summary>
/// XBOXゲームパッドの入力監視クラス
/// </summary>
class XboxPadInput
{
public:
	/// <summary>
	/// コンストラクタで取得するボタンを設定している
	/// </summary>
	XboxPadInput();
	~XboxPadInput();

	/// <summary>
	/// 入力情報更新
	/// </summary>
	void Update();
	/// <summary>
	/// ゲームパッドのキー番号から文字列に変換
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

#pragma region 振動
	/// <summary>
	/// バイブレーションを設定
	/// </summary>
	/// <param name="leftMotor">左モーター(低周波:重い振動)の強さ 0.0f ~ 1.0f</param>
	/// <param name="rightMotor">右モーター(高周波:軽い振動)の強さ 0.0f ~ 1.0f</param>
	void SetVibration(float leftMotor, float rightMotor) const;

	/// <summary>
	/// バイブレーションを停止
	/// </summary>
	void StopVibration();
#pragma endregion

private:
	// ゲームパッド入力情報格納
	// 一覧:https://learn.microsoft.com/ja-jp/windows/win32/inputdev/virtual-key-codes
	std::unordered_map<int, BOOL> _states;
	// 入力量がある情報
	std::unordered_map<int, float> _values;
	int					_slot = 0;
};