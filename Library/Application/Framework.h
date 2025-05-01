#pragma once

#include <d3d11.h>

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <string>
#include <WinUser.h>

#include "../Math/HighPrecisionTimer.h"

// ネットワーク
//#include "../Network/ServerAssignment.h"

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
	Framework(HWND hwnd) : _hwnd(hwnd) {}
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
	HighPrecisionTimer _tictoc = {};
	uint32_t _elapsedFrame{};
	float _elapsedTime{};
	float _fps{};

	void CalcFrameStatus();

	// 1秒が経過したフラグ
	bool _elapsed1Second = false;
private:
	// メインウィンドウのハンドル
	CONST HWND _hwnd = {};

	// 受信サーバー
	//ServerAssignment server;
};