#pragma once

#include <d3d11.h>

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <string>
#include <WinUser.h>

#include "../Math/HighPrecisionTimer.h"

CONST LONG SCREEN_WIDTH{ 1920 };
CONST LONG SCREEN_HEIGHT{ 1080 };
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
	Framework(HWND hwnd, HINSTANCE instance) : _hwnd(hwnd), _hInstance(instance) {}
	~Framework() {}

	// Frameworkのコピーを防ぐ
	Framework(const Framework&) = delete;
	Framework& operator=(const Framework&) = delete;
	Framework(Framework&&) noexcept = delete;
	Framework& operator=(Framework&&) noexcept = delete;

public:
	/// <summary>
	/// ゲームループの実行
	/// </summary>
	/// <returns></returns>
	int Run();

	/// <summary>
	/// ウィンドウのメッセージを受け取るためのコールバック関数
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="msg"></param>
	/// <param name="wparam"></param>
	/// <param name="lparam"></param>
	/// <returns></returns>
	LRESULT CALLBACK HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns></returns>
	bool Initialize() const;

	/// <summary>
	/// 毎フレームの更新処理
	/// </summary>
	/// <param name="elapsedTime"></param>
	void Update(float elapsedTime);

	/// <summary>
	/// 一定間隔の更新処理
	/// </summary>
	void FixedUpdate();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Render();

	/// <summary>
	/// 終了化
	/// </summary>
	/// <returns></returns>
	bool Uninitialize();

private:
	// フレームレートの計算
	HighPrecisionTimer _tictoc = {};
	uint32_t _elapsedFrame{};
	float _elapsedTime{};
	float _fps{};

	/// <summary>
	/// フレームレートの計算
	/// </summary>
	void CalcFrameStatus();

	// 固定間隔更新処理のためのタイマー
	float fixedUpdateTimer = 0.0f;
private:
	// メインウィンドウのハンドル
	CONST HWND _hwnd = {};
	// メインウィンドウのインスタンス
	CONST HINSTANCE _hInstance = {};
};