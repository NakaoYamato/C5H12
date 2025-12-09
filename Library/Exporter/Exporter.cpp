#include "Exporter.h"

#include "../../External/DirectXTex/DirectXTex.h"

#include <wrl.h>
#include <wincodec.h>
#include <fstream>

/// WAVファイルを保存する
void Exporter::SaveWavFile(
	const std::string& filename,
	const BYTE* data,
	DWORD dataSize,
	const WAVEFORMATEX& fmt)
{
    std::ofstream out(filename, std::ios::binary);

    // RIFFヘッダー
    out.write("RIFF", 4);
    uint32_t chunkSize = 36 + dataSize;
    out.write(reinterpret_cast<const char*>(&chunkSize), 4);
    out.write("WAVE", 4);

    // fmtチャンク
    out.write("fmt ", 4);
    uint32_t subchunk1Size = 16; // PCM
    out.write(reinterpret_cast<const char*>(&subchunk1Size), 4);
    out.write(reinterpret_cast<const char*>(&fmt.wFormatTag), 2);
    out.write(reinterpret_cast<const char*>(&fmt.nChannels), 2);
    out.write(reinterpret_cast<const char*>(&fmt.nSamplesPerSec), 4);
    out.write(reinterpret_cast<const char*>(&fmt.nAvgBytesPerSec), 4);
    out.write(reinterpret_cast<const char*>(&fmt.nBlockAlign), 2);
    out.write(reinterpret_cast<const char*>(&fmt.wBitsPerSample), 2);

    // dataチャンク
    out.write("data", 4);
    out.write(reinterpret_cast<const char*>(&dataSize), 4);
    out.write(reinterpret_cast<const char*>(data), dataSize);

    out.close();
}

/// SRVをPNGファイルとして保存する
bool Exporter::SavePngFile(
    ID3D11Device* device,
    ID3D11DeviceContext* dc,
    ID3D11ShaderResourceView* srv,
    const std::wstring& filePath)
{
    // ShaderResourceView からリソースを取得
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;
    srv->GetResource(&resource);

    // リソースが Texture2D か確認
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    HRESULT hr = resource.As(&texture);
    if (FAILED(hr)) return false;

    // テクスチャの情報を取得
    D3D11_TEXTURE2D_DESC desc = {};
    texture->GetDesc(&desc);

    // CPU 読み出し用ステージングテクスチャを作成
    D3D11_TEXTURE2D_DESC stagingDesc = desc;
    stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    stagingDesc.Usage = D3D11_USAGE_STAGING;
    stagingDesc.BindFlags = 0;
    stagingDesc.MiscFlags = 0;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> stagingTex;
    hr = device->CreateTexture2D(&stagingDesc, nullptr, &stagingTex);
    if (FAILED(hr)) return false;

    // リソースをステージングテクスチャへコピー
    dc->CopyResource(stagingTex.Get(), texture.Get());

    // DirectXTex の ScratchImage に変換
    DirectX::ScratchImage image;
    hr = DirectX::CaptureTexture(device, dc, stagingTex.Get(), image);
    if (FAILED(hr)) return false;

    // PNG として保存
    hr = DirectX::SaveToWICFile(
        image.GetImages(),        // 画像データ
        image.GetImageCount(),
        DirectX::WIC_FLAGS_FORCE_SRGB,
        GUID_ContainerFormatPng, // PNG フォーマット
        filePath.c_str()         // 出力先ファイル
    );

    return SUCCEEDED(hr);
}

/// SRVをDDSファイルとして保存する
bool Exporter::SaveDDSFile(ID3D11Device* device, 
    ID3D11DeviceContext* dc,
    ID3D11ShaderResourceView* srv,
    const std::wstring& filePath)
{
    using Microsoft::WRL::ComPtr;

    // SRVから元のテクスチャリソースを取得
    ComPtr<ID3D11Resource> pSourceResource;
    srv->GetResource(&pSourceResource);

    ComPtr<ID3D11Texture2D> pSourceTexture;
    HRESULT hr = pSourceResource.As(&pSourceTexture);
    if (FAILED(hr)) return false;

    D3D11_TEXTURE2D_DESC srcDesc;
    pSourceTexture->GetDesc(&srcDesc);

    // CPUが読み取れるステージングテクスチャを作成
    ComPtr<ID3D11Texture2D> pStagingTexture;
    D3D11_TEXTURE2D_DESC stagingDesc = srcDesc;
    stagingDesc.Usage = D3D11_USAGE_STAGING;
    stagingDesc.BindFlags = 0;
    stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    stagingDesc.MiscFlags = 0;

    hr = device->CreateTexture2D(&stagingDesc, nullptr, &pStagingTexture);
    if (FAILED(hr)) return false;

    // GPU上のテクスチャをステージングテクスチャにコピー
    dc->CopyResource(pStagingTexture.Get(), pSourceTexture.Get());

    // ステージングテクスチャをCPUにマップ
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    hr = dc->Map(pStagingTexture.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);
    if (FAILED(hr)) return false;

    // DirectX::Imageを構築してファイルに保存
    DirectX::Image image{};
    image.width = srcDesc.Width;
    image.height = srcDesc.Height;
    image.format = srcDesc.Format;
    image.rowPitch = mappedResource.RowPitch;
    image.slicePitch = mappedResource.DepthPitch;
    image.pixels = reinterpret_cast<uint8_t*>(mappedResource.pData);

    // DDSで保存
    hr = DirectX::SaveToDDSFile(image, DirectX::DDS_FLAGS_NONE, filePath.c_str());

    dc->Unmap(pStagingTexture.Get(), 0);

    return true;
}

bool Exporter::SaveJsonFile(const std::string& filename, const nlohmann::json& jsonData)
{
    // ディレクトリ確保
    std::filesystem::path outputDirPath(std::filesystem::path(filename).parent_path());
    if (!std::filesystem::exists(outputDirPath))
    {
        // なかったらディレクトリ作成
        std::filesystem::create_directories(outputDirPath);
    }

    std::ofstream writing_file;
    writing_file.open(filename, std::ios::out);
	if (writing_file.is_open())
	{
        writing_file << jsonData.dump() << std::endl;
        writing_file.close();

        return true;
	}
    else
    {
        return false; // ファイルが開けなかった場合は失敗
    }
}

bool Exporter::LoadJsonFile(const std::string& filename, nlohmann::json* jsonData)
{
    std::ifstream ifs(filename.c_str());
    if (ifs.good())
    {
        ifs >> *jsonData;
        return true;
    }
    else
    {
		return false;
    }
}
