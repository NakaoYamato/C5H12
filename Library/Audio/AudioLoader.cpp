#include "AudioLoader.h"

#include "../HRTrace.h"

#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif


// 読み込み
void AudioLoader::Load(const char* filePath, WAVEFORMATEXTENSIBLE& wfx, XAUDIO2_BUFFER& buffer)
{
	// ファイルを開く
	HANDLE hFile = CreateFileA(
		filePath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);

	_ASSERT_EXPR(INVALID_HANDLE_VALUE != hFile, HRTrace(HRESULT_FROM_WIN32(GetLastError())));
	_ASSERT_EXPR(INVALID_SET_FILE_POINTER != SetFilePointer(hFile, 0, NULL, FILE_BEGIN), HRTrace(HRESULT_FROM_WIN32(GetLastError())));


	// オーディオファイル内で"RIFF"チャンクを見つけて、ファイルの種類を確認する
	DWORD dwChunkSize;
	DWORD dwChunkPosition;

	// ファイルの種類を確認 fourccWAVE か 'XWMA'
	FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);

	DWORD filetype;
	ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
	_ASSERT_EXPR(filetype == fourccWAVE, L"ファイルの種類が.wavではありません");


	// "fmt"チャンクを見つけて、その内容をWAVEFORMATEXTENSIBLE構造体にコピー
	FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
	ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);


	// "data"チャンクを見つけて、内容をバッファに読み取る
	FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
	BYTE* pDataBuffer = new BYTE[dwChunkSize];
	ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);


	// バッファにデータを格納
	buffer.AudioBytes = dwChunkSize;
	buffer.pAudioData = pDataBuffer;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
}

// チャンクの検索
HRESULT AudioLoader::FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
{
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());

	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD bytesRead = 0;
	DWORD dwOffset = 0;

	while (hr == S_OK)
	{
		DWORD dwRead;
		if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());

		if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());

		switch (dwChunkType)
		{
		case fourccRIFF:
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
				hr = HRESULT_FROM_WIN32(GetLastError());
			break;

		default:
			if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
				return HRESULT_FROM_WIN32(GetLastError());
		}

		dwOffset += sizeof(DWORD) * 2;

		if (dwChunkType == fourcc)
		{
			dwChunkSize = dwChunkDataSize;
			dwChunkDataPosition = dwOffset;
			return S_OK;
		}

		dwOffset += dwChunkDataSize;

		if (bytesRead >= dwRIFFDataSize) return S_FALSE;

	}

	return S_OK;
}

// チャンクの読み取り
HRESULT AudioLoader::ReadChunkData(HANDLE hFile, void* buffer, DWORD bufferSize, DWORD bufferOffset)
{
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferOffset, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());

	DWORD dwRead;
	if (0 == ReadFile(hFile, buffer, bufferSize, &dwRead, NULL))
		hr = HRESULT_FROM_WIN32(GetLastError());

	return hr;
}
