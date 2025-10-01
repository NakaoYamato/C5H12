#pragma once

#include "../../Library/Graphics/GpuResourceManager.h"

class Texture
{
public:
	Texture() = default;
	~Texture() = default;

	// テクスチャを読み込む
	void Load(ID3D11Device* device, const wchar_t* filepath);
	// ダミーテクスチャ作成
	// <param name="device"></param>
	// <param name="value">0xAABBGGRRで表記する（例、赤:0xFF0000FF、青:0xFF00FF00）</param>
	// <param name="dimension">大きさ</param>
	void MakeDummyTexture(ID3D11Device* device,	DWORD value, UINT dimension);
	// SRVを設定
	void Set(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv);
	// SRVをコピー
	void Copy(ID3D11Device* device, ID3D11DeviceContext* dc, 
		ID3D11ShaderResourceView* sourceSRV, 
		D3D11_SHADER_RESOURCE_VIEW_DESC* srvDesc);

	// ファイルパスを取得
	const std::wstring& GetFilepath() const { return _filepath; }
	// 読み込んだテクスチャのSRVを取得
	ID3D11ShaderResourceView* Get() const { return _textureSRV.Get(); }
	// 読み込んだテクスチャのSRVを取得
	ID3D11ShaderResourceView* const* GetAddressOf() const { return _textureSRV.GetAddressOf(); }

	// 読み込んだテクスチャの2Dテクスチャ情報を取得
	const D3D11_TEXTURE2D_DESC& GetTexture2DDesc() const { return _texture2dDesc; }
private:
	std::wstring										_filepath{};
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	_textureSRV{};
	D3D11_TEXTURE2D_DESC								_texture2dDesc{};
};