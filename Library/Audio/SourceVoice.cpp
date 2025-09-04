#include "SourceVoice.h"

#include "../HRTrace.h"

// https://learn.microsoft.com/ja-jp/windows/win32/xaudio2/how-to--play-a-sound-with-xaudio2
SourceVoice::SourceVoice(IXAudio2* xAudio2, AudioResource* resource, SubmixVoice* submixVoice) :
	resource_(resource),
	submixVoice_(submixVoice)
{
	// ソースボイスの作成
	HRESULT hr = xAudio2->CreateSourceVoice(
		&xAudio2SourceVoice_,
		reinterpret_cast<WAVEFORMATEX*>(&resource_->GetWFX()),
		0,
		XAUDIO2_DEFAULT_FREQ_RATIO,
		0,
		submixVoice ? &submixVoice->GetSendList() : nullptr,
		nullptr
	);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));


#if 0
	Microsoft::WRL::ComPtr<IUnknown> pXAPO;
	hr = XAudio2CreateReverb(pXAPO.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	desc_.InitialState = true;
	desc_.OutputChannels = 2;
	desc_.pEffect = pXAPO.Get();

	chain_.EffectCount = 1;
	chain_.pEffectDescriptors = &desc_;

	hr = xAudio2SourceVoice_->SetEffectChain(&chain_);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	XAUDIO2FX_REVERB_PARAMETERS reverbParameters;
	reverbParameters.ReflectionsDelay = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_DELAY;
	reverbParameters.ReverbDelay = XAUDIO2FX_REVERB_DEFAULT_REVERB_DELAY;
	reverbParameters.RearDelay = XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY;
	reverbParameters.PositionLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION;
	reverbParameters.PositionRight = XAUDIO2FX_REVERB_DEFAULT_POSITION;
	reverbParameters.PositionMatrixLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
	reverbParameters.PositionMatrixRight = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
	reverbParameters.EarlyDiffusion = XAUDIO2FX_REVERB_DEFAULT_EARLY_DIFFUSION;
	reverbParameters.LateDiffusion = XAUDIO2FX_REVERB_DEFAULT_LATE_DIFFUSION;
	reverbParameters.LowEQGain = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_GAIN;
	reverbParameters.LowEQCutoff = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_CUTOFF;
	reverbParameters.HighEQGain = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_GAIN;
	reverbParameters.HighEQCutoff = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_CUTOFF;
	reverbParameters.RoomFilterFreq = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_FREQ;
	reverbParameters.RoomFilterMain = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_MAIN;
	reverbParameters.RoomFilterHF = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_HF;
	reverbParameters.ReflectionsGain = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_GAIN;
	reverbParameters.ReverbGain = XAUDIO2FX_REVERB_DEFAULT_REVERB_GAIN;
	reverbParameters.DecayTime = XAUDIO2FX_REVERB_DEFAULT_DECAY_TIME;
	reverbParameters.Density = XAUDIO2FX_REVERB_DEFAULT_DENSITY;
	reverbParameters.RoomSize = XAUDIO2FX_REVERB_DEFAULT_ROOM_SIZE;
	reverbParameters.WetDryMix = XAUDIO2FX_REVERB_DEFAULT_WET_DRY_MIX;
	hr = xAudio2SourceVoice_->SetEffectParameters(0, &reverbParameters, sizeof(reverbParameters));
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	hr = xAudio2SourceVoice_->EnableEffect(0);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
#endif
}

SourceVoice::~SourceVoice()
{
	Stop();
	xAudio2SourceVoice_->DestroyVoice();
	for (IXAudio2SourceVoice* sourceVoice : sourceVoices) { sourceVoice->DestroyVoice(); }
	sourceVoices.clear();
}

// フィルターパラメーターの適用
void SourceVoice::SetFilterParameters(XAUDIO2_FILTER_PARAMETERS* param)
{
	HRESULT hr = xAudio2SourceVoice_->SetFilterParameters(param);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

// 再生処理
void SourceVoice::Play(float vol)
{
	if (IsQueued()) return;

	HRESULT hr = xAudio2SourceVoice_->SetVolume(vol);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// バッファをソースに送信
	hr = xAudio2SourceVoice_->SubmitSourceBuffer(&resource_->GetBuffer());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// 再生
	hr = xAudio2SourceVoice_->Start(0);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

void SourceVoice::PlaySE(IXAudio2* xAudio2, float vol)
{
	for (IXAudio2SourceVoice* sourceVoice : sourceVoices) {
		XAUDIO2_VOICE_STATE state = {};
		sourceVoice->GetState(&state);
		if (state.BuffersQueued == 0) {
			sourceVoice->SubmitSourceBuffer(&resource_->GetBuffer());
			sourceVoice->SetVolume(vol);
			sourceVoice->Start();
			return;
		}
	}

	IXAudio2SourceVoice* sourceVoice;
	xAudio2->CreateSourceVoice(&sourceVoice, reinterpret_cast<WAVEFORMATEX*>(&resource_->GetWFX()));

	sourceVoice->SubmitSourceBuffer(&resource_->GetBuffer());
	sourceVoice->SetVolume(vol);
	sourceVoice->Start();
	sourceVoices.emplace_back(sourceVoice);
}

// 停止処理
void SourceVoice::Stop(bool flushBuffer)
{
	if (!IsQueued()) return;

	// 停止
	HRESULT hr = xAudio2SourceVoice_->Stop(0);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	if (flushBuffer)
	{
		// キューされているバッファを削除する
		hr = xAudio2SourceVoice_->FlushSourceBuffers();
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

void SourceVoice::StopSE(bool flushBuffer)
{
	auto StopFromPointer = [&](IXAudio2SourceVoice* voice)->void
		{
			XAUDIO2_VOICE_STATE state = {};
			voice->GetState(&state);
			if (state.BuffersQueued == 0)
				return;
			// 停止
			HRESULT hr = voice->Stop(0);
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

			if (flushBuffer)
			{
				// キューされているバッファを削除する
				hr = voice->FlushSourceBuffers();
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}
		};

	// 停止
	for (IXAudio2SourceVoice* sourceVoice : sourceVoices)
		StopFromPointer(sourceVoice);
}

// 再開処理
void SourceVoice::Resume()
{
	if (!IsQueued()) return;

	// 再生
	HRESULT hr = xAudio2SourceVoice_->Start(0);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

// 3Dオーディオを適用
void SourceVoice::Apply3DAudio(
	IXAudio2MasteringVoice* masterVoice,
	UINT32 dstChannels,
	const float* mat,
	float dopplerFactor,
	float reverbLevel
)
{
	// ボリュームを適用
	HRESULT hr = xAudio2SourceVoice_->SetOutputMatrix(
		masterVoice,
		1,
		dstChannels, 
		mat
	);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// ピッチを適用
	hr = xAudio2SourceVoice_->SetFrequencyRatio(dopplerFactor);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// リバーブレベルをサブミックスボイスに適用
	if (submixVoice_)
	{
		submixVoice_->Apply3DAudio(xAudio2SourceVoice_, &reverbLevel);
	}
}
