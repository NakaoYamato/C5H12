#include "MouseInput.h"

#include <algorithm>

MouseInputObserver::MouseInputObserver(HWND hWnd)
	: _hWnd(hWnd)
{
	// 画面サイズ設定
	RECT rc;
	GetClientRect(hWnd, &rc);
	_screenWidth = rc.right - rc.left;
	_screenHeight = rc.bottom - rc.top;

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
	::GetCursorPos(&_currentClientCursorPos);
	_currentScreenCursorPos = _currentClientCursorPos;
	::ScreenToClient(_hWnd, &_currentClientCursorPos);

	// 画面のサイズを取得する。
	RECT rc;
	GetClientRect(_hWnd, &rc);
	UINT screenW = rc.right - rc.left;
	UINT screenH = rc.bottom - rc.top;
	UINT viewportW = _screenWidth;
	UINT viewportH = _screenHeight;

	// 画面補正
	// 入力量がある情報の更新
	mouseParameters[MOUSE_OLD_POSITION_X] = mouseParameters[MOUSE_POSITION_X];
	mouseParameters[MOUSE_OLD_POSITION_Y] = mouseParameters[MOUSE_POSITION_Y];
	mouseParameters[MOUSE_POSITION_X] = (float)(_currentClientCursorPos.x / static_cast<float>(viewportW) * static_cast<float>(screenW));
	mouseParameters[MOUSE_POSITION_Y] = (float)(_currentClientCursorPos.y / static_cast<float>(viewportH) * static_cast<float>(screenH));

	mouseParameters[MOUSE_MOVE_X] = mouseParameters[MOUSE_POSITION_X] - mouseParameters[MOUSE_OLD_POSITION_X];
	mouseParameters[MOUSE_MOVE_Y] = mouseParameters[MOUSE_POSITION_Y] - mouseParameters[MOUSE_OLD_POSITION_Y];

	mouseParameters[MOUSE_AXIS_RX] = std::clamp<float>(mouseParameters[MOUSE_MOVE_X] / _normalizeFactor, -1.0f, 1.0f);
	mouseParameters[MOUSE_AXIS_RY] = std::clamp<float>(-mouseParameters[MOUSE_MOVE_Y] / _normalizeFactor, -1.0f, 1.0f);
}

/// マウスを画面中心に修正
void MouseInputObserver::FixCursorInCenter() const
{
	// ウィンドウの現在位置取得
	WINDOWINFO windowInfo{};
	GetWindowInfo(_hWnd, &windowInfo);

	// 画面の中心を取得する。
	int screenSenterX = _screenWidth / 2;
	int screenSenterY = _screenHeight / 2;

	SetCursorPos(screenSenterX + windowInfo.rcWindow.left, screenSenterY + +windowInfo.rcWindow.top);
}

/// マウスを画面内に修正
void MouseInputObserver::ClipCursorInWindow() const
{
	RECT rect;
	if (GetClientRect(_hWnd, &rect)) {
		POINT tl = { rect.left, rect.top };
		POINT br = { rect.right, rect.bottom };

		// クライアント座標をスクリーン座標に変換
		ClientToScreen(_hWnd, &tl);
		ClientToScreen(_hWnd, &br);

		RECT clipRect = { tl.x, tl.y, br.x, br.y };

		// マウスカーソルの移動をこの矩形に制限
		ClipCursor(&clipRect);
		// 即座に制限を解除することで移動量を取得できるようにする
		ClipCursor(nullptr);
	}
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
