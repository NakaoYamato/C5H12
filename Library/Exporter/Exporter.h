#pragma once

#include <d3d11.h>
#include <fstream>
#include <cstdint>
#include <xaudio2.h>

class Exporter
{
public:
    /// <summary>
	/// WAVƒtƒ@ƒCƒ‹‚ð•Û‘¶‚·‚é
    /// </summary>
    /// <param name="filename"></param>
    /// <param name="data"></param>
    /// <param name="dataSize"></param>
    /// <param name="fmt"></param>
    static void SaveWavFile(
        const std::string& filename,
        const BYTE* data,
        DWORD dataSize,
        const WAVEFORMATEX& fmt);

	static bool SavePngFile(
        ID3D11Device* device,
        ID3D11DeviceContext* dc,
        ID3D11ShaderResourceView* srv,
        const std::wstring& filePath);
};