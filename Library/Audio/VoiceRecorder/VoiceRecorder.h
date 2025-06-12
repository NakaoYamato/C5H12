#pragma once

#include <mmdeviceapi.h>
#include <audioclient.h>
#include <xaudio2.h>
#include <thread>
#include <memory>
#include <mutex>
#include <vector>

class VoiceRecorder
{
public:
    VoiceRecorder() {}
    ~VoiceRecorder() {}

    void StartRecording();
    void StopRecording();

private:
    void Recording();

private:
    bool _loop = true;
    std::mutex _mutex;
    std::unique_ptr<std::thread> _recordingThread;
    std::vector<XAUDIO2_BUFFER> _buffers = {};
};
