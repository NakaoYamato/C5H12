#pragma once

#include "../../Library/Graphics/GpuResourceManager.h"

class VertexShader
{
public:
	VertexShader() = default;
	~VertexShader() = default;
	// 頂点シェーダーを読み込む
	void Load(ID3D11Device* device, const std::string& filepath,
		D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements);
	// 読み込んだ頂点シェーダーを取得
	ID3D11VertexShader* Get();
	// 読み込んだ入力レイアウトを取得
	ID3D11InputLayout* GetInputLayout();
private:
	std::string _filepath{};
	ID3D11VertexShader* _vertexShader{};
	ID3D11InputLayout* _inputLayout{};
};

class PixelShader
{
public:
	PixelShader() = default;
	~PixelShader() = default;
	// ピクセルシェーダーを読み込む
	void Load(ID3D11Device* device, const std::string& filepath);
	// 読み込んだピクセルシェーダーを取得
	ID3D11PixelShader* Get();
private:
	std::string _filepath{};
	ID3D11PixelShader* _pixelShader{};
};