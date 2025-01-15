#include "MouseInput.h"

#include <algorithm>

MouseInputObserver::MouseInputObserver(HWND hWnd)
	: hWnd(hWnd)
{
	// 画面サイズ設定
	RECT rc;
	GetClientRect(hWnd, &rc);
	screenWidth = rc.right - rc.left;
	screenHeight = rc.bottom - rc.top;

	// 使用するキーを設定
	mousestates[VK_LBUTTON] = FALSE;
	mousestates[VK_MBUTTON] = FALSE;
	mousestates[VK_RBUTTON] = FALSE;

	mousestates[VK_XBUTTON1] = FALSE;
	mousestates[VK_XBUTTON2] = FALSE;

	mouseParameters[MOUSE_POSITION_X] = 0.0f;
	mouseParameters[MOUSE_POSITION_Y] = 0.0f;
	mouseParameters[MOUSE_OLD_POSITION_X] = 0.0f;
	mouseParameters[MOUSE_OLD_POSITION_Y] = 0.0f;
	mouseParameters[MOUSE_WHEEL] = 0.0f;
	mouseParameters[MOUSE_OLD_WHEEL] = 0.0f;

	mouseParameters[MOUSE_MOVE_X] = 0.0f;
	mouseParameters[MOUSE_MOVE_Y] = 0.0f;
	mouseParameters[MOUSE_AXIS_RX] = 0.0f;
	mouseParameters[MOUSE_AXIS_RY] = 0.0f;
}

/// 入力情報更新
void MouseInputObserver::Update()
{
	// 登録しているすべてのキー入力を確認
	for (auto& mousestate : mousestates)
	{
		mousestate.second = GetAsyncKeyState(mousestate.first) & 0x8000 ? TRUE : FALSE;
	}

	// 入力量がある情報の更新
	mouseParameters[MOUSE_OLD_WHEEL] = mouseParameters[MOUSE_WHEEL];
	mouseParameters[MOUSE_WHEEL] = 0.0f;

	/// マウス位置を更新
	UpdatePosition();
}

/// マウス位置を更新
void MouseInputObserver::UpdatePosition()
{
	// カーソル位置の取得
	POINT cursor;
	::GetCursorPos(&cursor);
	::ScreenToClient(hWnd, &cursor);

	// 画面のサイズを取得する。
	RECT rc;
	GetClientRect(hWnd, &rc);
	UINT screenW = rc.right - rc.left;
	UINT screenH = rc.bottom - rc.top;
	UINT viewportW = screenWidth;
	UINT viewportH = screenHeight;

	// 画面補正
	// 入力量がある情報の更新
	mouseParameters[MOUSE_OLD_POSITION_X] = mouseParameters[MOUSE_POSITION_X];
	mouseParameters[MOUSE_OLD_POSITION_Y] = mouseParameters[MOUSE_POSITION_Y];
	mouseParameters[MOUSE_POSITION_X] = (float)(cursor.x / static_cast<float>(viewportW) * static_cast<float>(screenW));
	mouseParameters[MOUSE_POSITION_Y] = (float)(cursor.y / static_cast<float>(viewportH) * static_cast<float>(screenH));

	mouseParameters[MOUSE_MOVE_X] = mouseParameters[MOUSE_POSITION_X] - mouseParameters[MOUSE_OLD_POSITION_X];
	mouseParameters[MOUSE_MOVE_Y] = mouseParameters[MOUSE_POSITION_Y] - mouseParameters[MOUSE_OLD_POSITION_Y];

	mouseParameters[MOUSE_AXIS_RX] = std::clamp<float>(mouseParameters[MOUSE_MOVE_X] / normalizeFactor, -1.0f, 1.0f);
	mouseParameters[MOUSE_AXIS_RY] = std::clamp<float>(-mouseParameters[MOUSE_MOVE_Y] / normalizeFactor, -1.0f, 1.0f);
}

/// マウスを画面内に修正
void MouseInputObserver::FixCursorInCenter() const
{
	// ウィンドウの現在位置取得
	WINDOWINFO windowInfo{};
	GetWindowInfo(hWnd, &windowInfo);

	// 画面の中心を取得する。
	RECT rc;
	GetClientRect(hWnd, &rc);
	int screenSenterX = screenWidth / 2;
	int screenSenterY = screenHeight / 2;

	SetCursorPos(screenSenterX + windowInfo.rcWindow.left, screenSenterY + +windowInfo.rcWindow.top);
}

/// マウスのキー番号から文字列に変換
const char* MouseInputObserver::ToString(int vKey)
{
	switch (vKey)
	{
	case VK_LBUTTON:	return u8"マウス左";
	case VK_MBUTTON:	return u8"マウス中";
	case VK_RBUTTON:	return u8"マウス右";

	case VK_XBUTTON1:	return u8"戻る";
	case VK_XBUTTON2:	return u8"進む";
	}
	return " ";
}
