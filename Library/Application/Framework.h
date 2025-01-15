#pragma once

#include <d3d11.h>

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <string>
#include <WinUser.h>

#include "../Math/HighPrecisionTimer.h"

CONST LONG SCREEN_WIDTH{ 1280 };
CONST LONG SCREEN_HEIGHT{ 720 };
CONST BOOL FULLSCREEN{ FALSE };
CONST LPCWSTR APPLICATION_NAME{ L"GAME" };

class Framework
{
public:
	// 垂直同期間隔設定
	static int syncInterval;

	// ウィンドウが受け取ったファイルパス
	static std::wstring filePath;

	// コンストラクタ
	Framework(HWND hwnd) : hwnd_(hwnd) {}
	~Framework() {}

	// Frameworkのコピーを防ぐ
	Framework(const Framework&) = delete;
	Framework& operator=(const Framework&) = delete;
	Framework(Framework&&) noexcept = delete;
	Framework& operator=(Framework&&) noexcept = delete;

public:
	// ゲームループの実行
	int Run();

	// ウィンドウのメッセージを受け取るためのコールバック関数
	LRESULT CALLBACK HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	bool Initialize() const;
	// 毎フレームの更新処理
	void Update(float elapsedTime/*Elapsed seconds from last frame*/);
	// 1秒ごとの更新処理
	void FixedUpdate();
	void Render(float elapsedTime/*Elapsed seconds from last frame*/);
	bool Uninitialize();

private:
	// フレームレートの計算
	HighPrecisionTimer tictoc_ = {};
	uint32_t elapsedFrame_{};
	float elapsedTime_{};
	float fps_{};

	void CalcFrameStatus();

	// 1秒が経過したフラグ
	bool elapsed1Second_ = false;
private:
	// メインウィンドウのハンドル
	CONST HWND hwnd_ = {};
};