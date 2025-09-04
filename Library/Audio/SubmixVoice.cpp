#include "SubmixVoice.h"
#include "../HrTrace.h"

// https://learn.microsoft.com/ja-jp/windows/win32/xaudio2/how-to--use-submix-voices
SubmixVoice::SubmixVoice(IXAudio2* xAudio2) :
	volume_(1.0f)
{
	// サブミックスボイスを作成
	xAudio2->CreateSubmixVoice(&xAudio2SubmixVoice_, 1, 44100, 0, 0, 0, 0);

	sfxSend_ = { 0, xAudio2SubmixVoice_ };
	sfxSendList_ = { 1, &sfxSend_ };
}

SubmixVoice::~SubmixVoice()
{
	xAudio2SubmixVoice_->DestroyVoice();
}

// 3Dオーディオの適用
void SubmixVoice::Apply3DAudio(IXAudio2SourceVoice* sourceVoice, const float* reverbLevel)
{
	HRESULT hr = sourceVoice->SetOutputMatrix(xAudio2SubmixVoice_, 1, 1, reverbLevel);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}
