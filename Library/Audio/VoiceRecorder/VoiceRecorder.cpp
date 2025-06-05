#include "VoiceRecorder.h"

VoiceRecorder::VoiceRecorder()
{
    // 1. ƒ}ƒCƒNŽæ“¾
    IMMDeviceEnumerator* pEnumerator = nullptr;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pEnumerator));
    IMMDevice* pDevice = nullptr;
    pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice);

    // 2. AudioClient Ý’è
    IAudioClient* pAudioClient = nullptr;
    pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&pAudioClient);

    WAVEFORMATEX* pwfx = nullptr;
    pAudioClient->GetMixFormat(&pwfx);
    pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 10000000, 0, pwfx, nullptr);

    // 3. CaptureClient Žæ“¾
    IAudioCaptureClient* pCaptureClient = nullptr;
    pAudioClient->GetService(IID_PPV_ARGS(&pCaptureClient));
    pAudioClient->Start();
}

VoiceRecorder::~VoiceRecorder()
{
}
