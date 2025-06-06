#pragma once

#include <d3d11.h>
#include <fstream>
#include <cstdint>
#include <xaudio2.h>
#include "../../External/json/json.hpp"

class Exporter
{
public:
    /// <summary>
	/// WAVファイルを保存する
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

	/// <summary>
	/// SRVをPNGファイルとして保存する
	/// </summary>
	/// <param name="device"></param>
	/// <param name="dc"></param>
	/// <param name="srv"></param>
	/// <param name="filePath"></param>
	/// <returns></returns>
	static bool SavePngFile(
        ID3D11Device* device,
        ID3D11DeviceContext* dc,
        ID3D11ShaderResourceView* srv,
        const std::wstring& filePath);

	/// <summary>
	/// JSONファイルを保存する
	/// </summary>
	/// <param name="filename"></param>
	/// <param name="jsonData"></param>
	/// <returns></returns>
	static bool SaveJsonFile(
		const std::string& filename,
		const nlohmann::json& jsonData);
	/// <summary>
	/// JSONファイルを読み込む
	/// </summary>
	/// <param name="filename"></param>
	/// <param name="jsonData"></param>
	/// <returns></returns>
	static bool LoadJsonFile(
		const std::string& filename,
		nlohmann::json* jsonData);
};