#pragma once

#include "../../Library/Graphics/GpuResourceManager.h"

// 頂点シェーダークラス
class VertexShader
{
public:
	VertexShader() = default;
	~VertexShader() = default;
	// 頂点シェーダーを読み込む
	void Load(ID3D11Device* device, const std::string& filepath,
		D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements);
	// 読み込んだ頂点シェーダーを取得
	ID3D11VertexShader* Get() const;
	// 読み込んだ入力レイアウトを取得
	ID3D11InputLayout* GetInputLayout() const;
private:
	std::string _filepath{};
	ID3D11VertexShader* _vertexShader{};
	ID3D11InputLayout* _inputLayout{};
};

// ピクセルシェーダークラス
class PixelShader
{
public:
	PixelShader() = default;
	~PixelShader() = default;
	// ピクセルシェーダーを読み込む
	void Load(ID3D11Device* device, const std::string& filepath);
	// 読み込んだピクセルシェーダーを取得
	ID3D11PixelShader* Get() const;
private:
	std::string _filepath{};
	ID3D11PixelShader* _pixelShader{};
};

// ジオメトリシェーダークラス
class GeometryShader
{
public:
	GeometryShader() = default;
	~GeometryShader() = default;
	// ジオメトリシェーダーを読み込む
	void Load(ID3D11Device* device, const std::string& filepath);
	// 読み込んだジオメトリシェーダーを取得
	ID3D11GeometryShader* Get() const;
private:
	std::string _filepath{};
	ID3D11GeometryShader* _geometryShader{};
};

// ハルシェーダークラス
class HullShader
{
public:
	HullShader() = default;
	~HullShader() = default;
	// ハルシェーダーを読み込む
	void Load(ID3D11Device* device, const std::string& filepath);
	// 読み込んだハルシェーダーを取得
	ID3D11HullShader* Get() const;
private:
	std::string _filepath{};
	Microsoft::WRL::ComPtr<ID3D11HullShader> _hullShader{};
};

// ドメインシェーダークラス
class DomainShader
{
public:
	DomainShader() = default;
	~DomainShader() = default;
	// ドメインシェーダーを読み込む
	void Load(ID3D11Device* device, const std::string& filepath);
	// 読み込んだドメインシェーダーを取得
	ID3D11DomainShader* Get() const;
private:
	std::string _filepath{};
	Microsoft::WRL::ComPtr<ID3D11DomainShader> _domainShader{};
};