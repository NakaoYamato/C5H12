#pragma once

#include <vector>

#include "AudioResource.h"
#include "SubmixVoice.h"

#include <xaudio2.h>
#include <xaudio2fx.h>

class SourceVoice
{
public:
	SourceVoice(IXAudio2* xAudio2, AudioResource* resource, SubmixVoice* submixVoice);
	~SourceVoice();

	// 再生中かどうか
	bool IsQueued() const 
	{
		XAUDIO2_VOICE_STATE state = {};
		xAudio2SourceVoice_->GetState(&state);
		return state.BuffersQueued;
	}
	bool IsQueuedSE() const 
	{
		for (IXAudio2SourceVoice* sourceVoice : sourceVoices) {
			XAUDIO2_VOICE_STATE state = {};
			sourceVoice->GetState(&state);
			if (state.BuffersQueued != 0)
				return true;
		}

		return false;
	}

	// グループが同じかどうか
	bool IsSameGroup(SubmixVoice* group) { return submixVoice_ == group; }

	// フィルターパラメーターの適用
	void SetFilterParameters(XAUDIO2_FILTER_PARAMETERS* param);

	// 再生処理
	void Play(float vol);
	void PlaySE(IXAudio2* xAudio2, float vol);// TODO : 設計見直す

	// 停止処理
	void Stop(bool flushBuffer = true);
	void StopSE(bool flushBuffer = true);

	// 再開処理
	void Resume();

	// 3Dオーディオを適用
	void Apply3DAudio(
		IXAudio2MasteringVoice* masterVoice,
		UINT32 dstChannels,
		const float* mat,
		float dopplerFactor,
		float reverbLevel
	);

private:
	IXAudio2SourceVoice* xAudio2SourceVoice_;
	std::vector<IXAudio2SourceVoice*> sourceVoices;
	AudioResource* resource_;
	SubmixVoice* submixVoice_;

	XAUDIO2_EFFECT_DESCRIPTOR desc_;
	XAUDIO2_EFFECT_CHAIN chain_;
};