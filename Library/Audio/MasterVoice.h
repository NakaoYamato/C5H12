#pragma once

#include <memory>

#include <xaudio2.h>
#include <x3daudio.h>
#include <wrl.h>

class MasterVoice
{
public:
	MasterVoice();

	IXAudio2* GetXAudio2() const { return xAudio2_.Get(); }
	IXAudio2MasteringVoice* GetMasteringVoice() const { return xAudio2MasteringVoice_; }
	float GetVolume() const { return volume_; }

	void SetVolume(float vol) { volume_ = vol; xAudio2MasteringVoice_->SetVolume(vol); }

private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* xAudio2MasteringVoice_;

	float volume_;
};