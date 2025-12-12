#include "Shader.h"

// 頂点シェーダーを読み込む
void VertexShader::Load(ID3D11Device* device, const std::string& filepath, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements)
{
	_filepath = filepath;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	GpuResourceManager::CreateVsFromCso(
		device,
		_filepath.c_str(),
		vertexShader.ReleaseAndGetAddressOf(),
		inputLayout.ReleaseAndGetAddressOf(),
		inputElementDesc,
		numElements
	);
	_vertexShader = GpuResourceManager::GetVertexShader(_filepath).Get();
	_inputLayout = GpuResourceManager::GetInputLayout(_filepath).Get();
}

ID3D11VertexShader* VertexShader::Get() const
{
#ifdef _DEBUG
	return GpuResourceManager::GetVertexShader(_filepath).Get();
#else
	return _vertexShader;
#endif // _DEBUG
}

// 読み込んだ入力レイアウトを取得
ID3D11InputLayout* VertexShader::GetInputLayout() const
{
#ifdef _DEBUG
	return GpuResourceManager::GetInputLayout(_filepath).Get();
#else
	return _inputLayout;
#endif // _DEBUG
}

// ピクセルシェーダーを読み込む
void PixelShader::Load(ID3D11Device* device, const std::string& filepath)
{
	_filepath = filepath;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	GpuResourceManager::CreatePsFromCso(
		device,
		_filepath.c_str(),
		pixelShader.ReleaseAndGetAddressOf()
	);
	_pixelShader = GpuResourceManager::GetPixelShader(_filepath).Get();
}

// 読み込んだピクセルシェーダーを取得
ID3D11PixelShader* PixelShader::Get() const
{
#ifdef _DEBUG
	return GpuResourceManager::GetPixelShader(_filepath).Get();
#else
	return _pixelShader;
#endif // _DEBUG
}

// ジオメトリシェーダーを読み込む
void GeometryShader::Load(ID3D11Device* device, const std::string& filepath)
{
	_filepath = filepath;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometryShader;
	GpuResourceManager::CreateGsFromCso(
		device,
		_filepath.c_str(),
		geometryShader.ReleaseAndGetAddressOf()
	);
	_geometryShader = GpuResourceManager::GetGeometryShader(_filepath).Get();
}

// 読み込んだジオメトリシェーダーを取得
ID3D11GeometryShader* GeometryShader::Get() const
{
#ifdef _DEBUG
	return GpuResourceManager::GetGeometryShader(_filepath).Get();
#else
	return _geometryShader;
#endif // _DEBUG
}
