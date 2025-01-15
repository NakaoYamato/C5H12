#pragma once

#include <windows.h>

// 経過時間の計算
class HighPrecisionTimer
{
public:
	HighPrecisionTimer()
	{
		// 高精度のタイマーをつくる
		LONGLONG countsPerSec{};
		// 精度を取得する
		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&countsPerSec));
		secondsPerCount = 1.0 / static_cast<double>(countsPerSec);

		// 現在のカウンタ値を取得
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&thisTime));
		baseTime = thisTime;
		lastTime = thisTime;
	}

	// コピーを防ぐ
	~HighPrecisionTimer() = default;
	HighPrecisionTimer(const HighPrecisionTimer&) = delete;
	HighPrecisionTimer& operator=(const HighPrecisionTimer&) = delete;
	HighPrecisionTimer(HighPrecisionTimer&&) noexcept = delete;
	HighPrecisionTimer& operator=(HighPrecisionTimer&&) noexcept = delete;

	// Reset関数が呼び出されてからの経過時間（ｓ）を取得
	float TimeStamp()const
	{
		if (stopped)
		{
			// 止まっている時間を返す
			//                     |<--pausedTime-->|
			// ----*---------------*-----------------*------------*------------*------> time
			//  baseTime       stopTime        startTime     stopTime    thisTime
			return static_cast<float>(
				((stopTime - pausedTime) - baseTime) * secondsPerCount
			);
		}
		else
		{
			// 経過時間を返す
			//                     |<--pausedTime-->|
			// ----*---------------*-----------------*------------*------> time
			//  baseTime       stopTime        startTime     thisTime
			return static_cast<float>(
				((thisTime - pausedTime) - baseTime) * secondsPerCount
			);
		}
	}

	// 経過時間（elapsedTime）(s)を取得
	float TimeInterval() const
	{
		return static_cast<float>(deltaTime);
	}

	// メッセージループの前に呼び出す
	void Reset()
	{
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&thisTime));
		baseTime = thisTime;
		lastTime = thisTime;

		stopTime = 0;
		stopped = false;
	}

	// ウィンドウがアクティブになったら呼び出す
	void Start()
	{
		LONGLONG startTime{};
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&startTime));

		//
		//                     |<-------d------->|
		// ----*---------------*-----------------*------------> time
		//  baseTime       stopTime        startTime     
		if (stopped)
		{
			pausedTime += (startTime - stopTime);
			lastTime = startTime;
			stopTime = 0;
			stopped = false;
		}
	}

	// ウィンドウが非アクティブになったら呼び出す
	void Stop() 
	{
		if (!stopped)
		{
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&stopTime));
			stopped = true;
		}
	}

	void Tick() // Call every frame.
	{
		if (stopped)
		{
			deltaTime = 0.0;
			return;
		}

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&thisTime));
		// このフレームと前のフレームとの時間差。
		deltaTime = (thisTime - lastTime) * secondsPerCount;

		// 最終時間を更新
		lastTime = thisTime;

		// delataTimeが負になる場合があるので対応
		if (deltaTime < 0.0)
		{
			deltaTime = 0.0;
		}
	}
private:
	double secondsPerCount{ 0.0 };
	double deltaTime{ 0.0 };

	LONGLONG baseTime{};
	LONGLONG pausedTime{};
	LONGLONG stopTime{};
	LONGLONG lastTime{};
	LONGLONG thisTime{};

	bool stopped = false;
};