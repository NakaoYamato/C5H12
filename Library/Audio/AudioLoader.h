#pragma once

#include <xaudio2.h>

// https://learn.microsoft.com/ja-jp/windows/win32/xaudio2/how-to--load-audio-data-files-in-xaudio2
class AudioLoader
{
public:
	// 読み込み
	static void Load(const char* filePath, WAVEFORMATEXTENSIBLE& wfx, XAUDIO2_BUFFER& buffer);

private:
	// チャンクの検索
	static HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition);

	// チャンクの読み取り
	static HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD bufferSize, DWORD bufferOffset);
};