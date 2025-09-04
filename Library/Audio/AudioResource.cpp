#include "AudioResource.h"

#include "AudioLoader.h"

AudioResource::AudioResource(IXAudio2* xAudio2, const char* filePath, int loopCount)
{
	// オーディオファイルの読み込み
	AudioLoader::Load(filePath, wfx_, buffer_);

	// ループ数の設定
	buffer_.LoopCount = loopCount;
}

AudioResource::~AudioResource()
{
	delete[] buffer_.pAudioData;
}