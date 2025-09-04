#pragma once

#include <Windows.h>
#include <unordered_map>

static constexpr int MOUSE_POSITION_X = 0x00;
static constexpr int MOUSE_POSITION_Y = 0x01;
static constexpr int MOUSE_OLD_POSITION_X = 0x02;
static constexpr int MOUSE_OLD_POSITION_Y = 0x03;
static constexpr int MOUSE_WHEEL = 0x04;
static constexpr int MOUSE_OLD_WHEEL = 0x05;

static constexpr int MOUSE_MOVE_X = 0x06;
static constexpr int MOUSE_MOVE_Y = 0x07;

static constexpr int MOUSE_AXIS_RX = 0x08;
static constexpr int MOUSE_AXIS_RY = 0x09;

/// <summary>
/// マウスの入力監視クラス
/// </summary>
class MouseInput
{
public:
	/// <summary>
	/// コンストラクタで取得するボタンを設定している
	/// </summary>
	MouseInput(HWND hWnd);
	~MouseInput() {}

	/// <summary>
	/// 入力情報更新
	/// </summary>
	void Update();
	/// <summary>
	/// マウス位置を更新
	/// </summary>
	void UpdatePosition();
	/// <summary>
	/// ホイール移動値をセット
	/// </summary>
	void SetWheel(int wheel) { this->_values[MOUSE_WHEEL] += wheel; }
	/// <summary>
	/// マウスを画面中心に修正
	/// </summary>
	void FixCursorInCenter() const;
	/// <summary>
	/// マウスを画面内に修正
	/// </summary>
	void ClipCursorInWindow() const;
	/// <summary>
	/// マウスのキー番号から文字列に変換
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
	// マウス入力情報格納
	// 一覧:https://learn.microsoft.com/ja-jp/windows/win32/inputdev/virtual-key-codes
	std::unordered_map<int, BOOL> _states;
	// 入力量がある情報
	std::unordered_map<int, float> _values;

	int				_screenWidth = 0;
	int				_screenHeight = 0;
	HWND			_hWnd = nullptr;

	// マウス移動量を正規化するためのパラメータ
	float _normalizeFactor = 10.0f;

	// 現在のカーソル位置
	POINT _currentScreenCursorPos = { 0, 0 };
	POINT _currentClientCursorPos = { 0, 0 };
};