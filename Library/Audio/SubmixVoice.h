#pragma once

#include <xaudio2.h>
#include <wrl.h>

class SubmixVoice
{
public:
	SubmixVoice(IXAudio2* xAudio2);
	~SubmixVoice();

	XAUDIO2_VOICE_SENDS& GetSendList() { return sfxSendList_; }
	float GetVolume() const { return volume_; }

	void SetVolume(float vol) { volume_ = vol; xAudio2SubmixVoice_->SetVolume(vol); }

	// 3Dオーディオの適用
	void Apply3DAudio(IXAudio2SourceVoice* sourceVoice, const float* reverbLevel);

private:
	IXAudio2SubmixVoice*	xAudio2SubmixVoice_;
	XAUDIO2_SEND_DESCRIPTOR sfxSend_;
	XAUDIO2_VOICE_SENDS		sfxSendList_;

	float volume_;
};