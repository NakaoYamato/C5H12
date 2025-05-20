#pragma once

#include <string>
#include <unordered_map>
#include <wrl.h>
#include <d3d11.h>

#include "../../Library/Math/Vector.h"

/// <summary>
/// ブレンドタイプ
/// </summary>
enum class BlendType
{
	Opaque,
	Alpha,

	BlendTypeMax
};

/// <summary>
/// モデルのシェーダータイプ
/// </summary>
enum class ModelShaderType
{
	Phong,
	Ramp,
	Grass,
	PBR,

	CascadedShadowMap,

	ModelShaderTypeMax
};

class Material
{
public:
	struct TextureData
	{
		std::wstring filename;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;
		D3D11_TEXTURE2D_DESC texture2dDesc{};
	};
public:
	Material() = default;
	Material(const std::string& name) : _name(name) {}
	~Material() = default;

	/// <summary>
	/// デバッグGUI表示
	/// </summary>
	void DrawGui();

	/// <summary>
	/// ファイルからテクスチャを読み込む
	/// </summary>
	/// <param name="device"></param>
	/// <param name="key"></param>
	/// <param name="filename"></param>
	void LoadTexture(ID3D11Device* device,
		const std::string& key,
		const wchar_t* filename);

	/// <summary>
	/// ダミーテクスチャ作成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="key"></param>
	/// <param name="value"></param>
	/// <param name="dimension"></param>
	void MakeDummyTexture(ID3D11Device* device,
		const std::string& key,
		DWORD value,
		UINT dimension);
	
	/// <summary>
	/// 指定のマテリアルのSRVを変更
	/// </summary>
	/// <param name="srv"></param>
	/// <param name="key"></param>
	void ChangeTextureSRV(
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv,
		const std::string& key)
	{
		_textureDatas[key].textureSRV = srv;
	}
#pragma region アクセサ
	const std::string& GetName() const { return _name; }
	const Vector4& GetColor(const std::string& key) const { return _colors.at(key); }
	const TextureData& GetTextureData(const std::string& key) const { return _textureDatas.at(key); }
	ID3D11ShaderResourceView* const* GetAddressOfTextureSRV(const std::string& key) const { return _textureDatas.at(key).textureSRV.GetAddressOf(); }
	ID3D11ShaderResourceView* GetTextureSRV(const std::string& key) const { return _textureDatas.at(key).textureSRV.Get(); }
	BlendType GetBlendType() const { return _blendType; }
	ModelShaderType GetShaderType() const { return _shaderType; }

	void SetName(const std::string& name) { _name = name; }
	void SetColor(const std::string& key, const Vector4& color) { _colors[key] = color; }
	void SetTextureData(const std::string& key, const TextureData& textureData) { _textureDatas[key] = textureData; }
	void SetBlendType(BlendType type) { _blendType = type; }
	void SetShaderType(ModelShaderType type);
#pragma endregion

private:
	std::string										_name;
	std::unordered_map<std::string, Vector4>		_colors;
	std::unordered_map<std::string, TextureData>	_textureDatas;

	BlendType		_blendType = BlendType::Opaque;
	ModelShaderType	_shaderType = ModelShaderType::PBR;
};