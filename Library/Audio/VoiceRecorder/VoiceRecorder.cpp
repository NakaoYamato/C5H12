#include "VoiceRecorder.h"

void VoiceRecorder::StartRecording()
{
    _loop = true;
    // スレッド開始 
    _recordingThread = std::make_unique<std::thread>(&VoiceRecorder::Recording, this);
}

void VoiceRecorder::StopRecording()
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _loop = false; // ループ終了フラグを設定
    }
    if (_recordingThread && _recordingThread->joinable()) {
        _recordingThread->join(); // スレッドの終了を待機
    }
}

void VoiceRecorder::Recording()
{
    // 1. マイク取得
    IMMDeviceEnumerator* pEnumerator = nullptr;
    (void)CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pEnumerator));
    IMMDevice* pDevice = nullptr;
    pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice);

    // 2. AudioClient 設定
    IAudioClient* pAudioClient = nullptr;
    pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&pAudioClient);

    WAVEFORMATEX* pwfx = nullptr;
    pAudioClient->GetMixFormat(&pwfx);
    pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 10000000, 0, pwfx, nullptr);

    // 3. CaptureClient 取得
    IAudioCaptureClient* pCaptureClient = nullptr;
    pAudioClient->GetService(IID_PPV_ARGS(&pCaptureClient));
    pAudioClient->Start();

    // 5. キャプチャループ
    while (true)
    {
        UINT32 packetLength = 0;
        pCaptureClient->GetNextPacketSize(&packetLength);
        if (packetLength > 0) {
            XAUDIO2_BUFFER xaBuf = {};
            BYTE* pData;
            UINT32 numFrames;
            DWORD flags;
            pCaptureClient->GetBuffer(&pData, &numFrames, &flags, nullptr, nullptr);
            pCaptureClient->ReleaseBuffer(numFrames);
        }
        Sleep(10);

        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (!_loop) {
                break; // ループ終了
            }
        }
    }

}
