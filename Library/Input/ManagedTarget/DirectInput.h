#pragma once
#include <dinput.h>
#include <tchar.h>
#include <unordered_map>

#pragma region ボタンを取得する用の定義
static constexpr int DIRECTPAD_BUTTON_R_LEFT	= 0x00;
static constexpr int DIRECTPAD_BUTTON_R_DOWN	= 0x01;
static constexpr int DIRECTPAD_BUTTON_R_RIGHT	= 0x02;
static constexpr int DIRECTPAD_BUTTON_R_UP		= 0x03;
static constexpr int DIRECTPAD_BUTTON_L1		= 0x04;
static constexpr int DIRECTPAD_BUTTON_R1		= 0x05;
static constexpr int DIRECTPAD_BUTTON_L2		= 0x06;
static constexpr int DIRECTPAD_BUTTON_R2		= 0x07;
static constexpr int DIRECTPAD_BUTTON_BACK		= 0x08;
static constexpr int DIRECTPAD_BUTTON_START		= 0x09;
static constexpr int DIRECTPAD_BUTTON_L3		= 0x0A;
static constexpr int DIRECTPAD_BUTTON_R3		= 0x0B;
static constexpr int DIRECTPAD_BUTTON_L_LEFT	= 0x0C;
static constexpr int DIRECTPAD_BUTTON_L_DOWN	= 0x0D;
static constexpr int DIRECTPAD_BUTTON_L_RIGHT	= 0x0E;
static constexpr int DIRECTPAD_BUTTON_L_UP		= 0x0F;
#pragma endregion

#pragma region 入力値を取得する用の定義
static constexpr int DIRECTPAD_AXIS_LX = 0x00;
static constexpr int DIRECTPAD_AXIS_LY = 0x01;
static constexpr int DIRECTPAD_AXIS_RX = 0x02;
static constexpr int DIRECTPAD_AXIS_RY = 0x03;
#pragma endregion

class DirectInput
{
public:
	// DirectInputのゲームパッドを検索するためのコールバック関数
	static BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);

public:
	DirectInput(HWND hwnd, HINSTANCE instance);
	~DirectInput();
	/// <summary>
	/// 入力情報更新
	/// </summary>
	void Update();
	// ゲームパッドを検索
	void SearchGamepad();
	/// <summary>
	/// ボタン番号から文字列に変換
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
	/// <summary>
	/// 入力情報をクリアする
	/// </summary>
	void ClearInput();
	/// <summary>
	/// ジョイスティックの軸値をデッドゾーン処理する
	/// </summary>
	/// <param name="axisValue"></param>
	/// <returns></returns>
	float GetJoystickAxisValue(LONG axisValue) const;
	/// <summary>
	/// ゲームパッド情報を解放する
	/// </summary>
	void ReleaseGamepad();
private:
	// メインウィンドウのハンドル
	HWND _hwnd{};
	// DirectInputのインターフェース
	LPDIRECTINPUT8 _directInput = nullptr;
	// DirectInputのゲームパッド
	LPDIRECTINPUTDEVICE8 _gamepad = nullptr;
	// 入力情報格納
	std::unordered_map<int, BOOL> _states;
	// 入力量がある情報
	std::unordered_map<int, float> _values;

	// ジョイスティックのデッドゾーンの閾値
	float _joystickDeadZone = 0.1f;
	// 十字キーのデッドゾーンの閾値
	DWORD _povDeadZone = 2000;
};