#pragma once

#include <mmdeviceapi.h>
#include <audioclient.h>
#include <xaudio2.h>

class VoiceRecorder
{
public:
    VoiceRecorder();
    ~VoiceRecorder();

private:
    XAUDIO2_BUFFER _buffer = {};
};
