#pragma once

#include <xaudio2.h>

class AudioResource
{
public:
	AudioResource(IXAudio2* xAudio2, const char* filePath, int loopCount = 0);
	~AudioResource();

	WAVEFORMATEXTENSIBLE& GetWFX() { return wfx_; }
	XAUDIO2_BUFFER& GetBuffer() { return buffer_; }

private:
	WAVEFORMATEXTENSIBLE wfx_ = {};
	XAUDIO2_BUFFER buffer_ = {};
};