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
		_secondsPerCount = 1.0 / static_cast<double>(countsPerSec);

		// 現在のカウンタ値を取得
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_thisTime));
		_baseTime = _thisTime;
		_lastTime = _thisTime;
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
		if (_stopped)
		{
			// 止まっている時間を返す
			//                     |<--pausedTime-->|
			// ----*---------------*-----------------*------------*------------*------> time
			//  baseTime       stopTime        startTime     stopTime    thisTime
			return static_cast<float>(
				((_stopTime - _pausedTime) - _baseTime) * _secondsPerCount
			);
		}
		else
		{
			// 経過時間を返す
			//                     |<--pausedTime-->|
			// ----*---------------*-----------------*------------*------> time
			//  baseTime       stopTime        startTime     thisTime
			return static_cast<float>(
				((_thisTime - _pausedTime) - _baseTime) * _secondsPerCount
			);
		}
	}

	// 経過時間（elapsedTime）(s)を取得
	float TimeInterval() const
	{
		return static_cast<float>(_deltaTime);
	}

	// メッセージループの前に呼び出す
	void Reset()
	{
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_thisTime));
		_baseTime = _thisTime;
		_lastTime = _thisTime;

		_stopTime = 0;
		_stopped = false;
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
		if (_stopped)
		{
			_pausedTime += (startTime - _stopTime);
			_lastTime = startTime;
			_stopTime = 0;
			_stopped = false;
		}
	}

	// ウィンドウが非アクティブになったら呼び出す
	void Stop() 
	{
		if (!_stopped)
		{
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_stopTime));
			_stopped = true;
		}
	}

	void Tick() // Call every frame.
	{
		if (_stopped)
		{
			_deltaTime = 0.0;
			return;
		}

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_thisTime));
		// このフレームと前のフレームとの時間差。
		_deltaTime = (_thisTime - _lastTime) * _secondsPerCount;

		// 最終時間を更新
		_lastTime = _thisTime;

		// delataTimeが負になる場合があるので対応
		if (_deltaTime < 0.0)
		{
			_deltaTime = 0.0;
		}
	}

	float GetMilliseconds() const
	{
		LONGLONG counter;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&counter));
		float ms = (float)(1000.0f * (counter - _baseTime) * _secondsPerCount);
		return ms;
	}
private:
	double _secondsPerCount{ 0.0 };
	double _deltaTime{ 0.0 };

	LONGLONG _baseTime{};
	LONGLONG _pausedTime{};
	LONGLONG _stopTime{};
	LONGLONG _lastTime{};
	LONGLONG _thisTime{};

	bool _stopped = false;
};