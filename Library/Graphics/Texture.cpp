#include "Texture.h"

#include "../../Library/DebugSupporter/DebugSupporter.h"

// テクスチャを読み込む
void Texture::Load(ID3D11Device* device, const wchar_t* filepath)
{
	_filepath = filepath;
	bool res = GpuResourceManager::LoadTextureFromFile(device, filepath,
		_textureSRV.ReleaseAndGetAddressOf(),
		&_texture2dDesc);

	// 読み込みに失敗した場合はダミーテクスチャを作成
	if (!res)
	{
		Debug::Output::String(L"テクスチャの読み込みに失敗しました: ");
		Debug::Output::String(filepath);
		Debug::Output::String(L"\n");
		// 赤色の1x1ダミーテクスチャを作成
		MakeDummyTexture(device, 0xFF0000FF, 1);
	}
}

// ダミーテクスチャ作成
void Texture::MakeDummyTexture(ID3D11Device* device, DWORD value, UINT dimension)
{
	_filepath = L"";
	GpuResourceManager::MakeDummyTexture(device,
		_textureSRV.ReleaseAndGetAddressOf(),
		&_texture2dDesc,
		value, dimension);
}

// SRVを設定
void Texture::Set(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
	_textureSRV = srv;
}

// SRVをコピー
void Texture::Copy(ID3D11Device* device, ID3D11DeviceContext* dc, ID3D11ShaderResourceView* sourceSRV, D3D11_SHADER_RESOURCE_VIEW_DESC* srvDesc)
{
	GpuResourceManager::CreateShaderResourceViewCopy(device, dc, sourceSRV, srvDesc, _textureSRV.ReleaseAndGetAddressOf());
}
