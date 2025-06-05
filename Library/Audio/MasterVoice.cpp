#include "MasterVoice.h"
#include "../HRTrace.h"

MasterVoice::MasterVoice() :
	volume_(1.0f)
{
	// https://learn.microsoft.com/ja-jp/windows/win32/xaudio2/how-to--initialize-xaudio2
	// XAudio2エンジンのインスタンスを作成
	HRESULT hr = ::XAudio2Create(xAudio2_.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// マスターボイスを作成
	hr = xAudio2_->CreateMasteringVoice(&xAudio2MasteringVoice_);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}