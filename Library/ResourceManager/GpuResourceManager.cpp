#include "GpuResourceManager.h"
#include <filesystem>
#include <wrl.h>

#include <string>
#include <map>
#include <memory>

#include "../../External/DirectXTex/DDS.h"
#include "../../External/DirectXTK-main/WICTextureLoader.h"
#include "../../External/DirectXTK-main/DDSTextureLoader.h"
#include "../HRTrace.h"

#include "../DebugSupporter/DebugSupporter.h"

// 頂点シェーダー読み込み
HRESULT GpuResourceManager::LoadVertexShader(
	ID3D11Device* device,
	const char* filename,
	const D3D11_INPUT_ELEMENT_DESC inputElementDescs[],
	UINT inputElementCount,
	ID3D11InputLayout** inputLayout,
	ID3D11VertexShader** vertexShader)
{
	// ファイルを開く
	FILE* fp = nullptr;
	fopen_s(&fp, filename, "rb");
	_ASSERT_EXPR_A(fp, "Vertex Shader File not found");

	// ファイルのサイズを求める
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// メモリ上に頂点シェーダーデータを格納する領域を用意する
	std::unique_ptr<u_char[]> data = std::make_unique<u_char[]>(size);
	fread(data.get(), size, 1, fp);
	fclose(fp);

	// 頂点シェーダー生成
	HRESULT hr = device->CreateVertexShader(data.get(), size, nullptr, vertexShader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// 入力レイアウト
	if (inputLayout != nullptr)
	{
		hr = device->CreateInputLayout(inputElementDescs, inputElementCount, data.get(), size, inputLayout);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	return hr;
}

// 頂点シェーダ作成
void GpuResourceManager::CreateVsFromCso(ID3D11Device* device, 
	const char* csoName,
	ID3D11VertexShader** vertexShader, 
	ID3D11InputLayout** inputLayout, 
	D3D11_INPUT_ELEMENT_DESC* inputElementDesc, 
	UINT numElements)
{
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11VertexShader>> vertexShaderData;
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11InputLayout>> inputLayoutData;

	HRESULT hr = { S_OK };

	// 過去に生成しているか確認
	auto it = vertexShaderData.find(csoName);
	if (it != vertexShaderData.end())
	{
		// 過去に生成したデータを取得
		*vertexShader = it->second.Get();
		(*vertexShader)->AddRef();

		auto inputIter = inputLayoutData.find(csoName);
		if (inputIter != inputLayoutData.end())
		{
			*inputLayout = inputIter->second.Get();
			(*inputLayout)->AddRef();
		}
	}
	else
	{
		// 頂点シェーダーオブジェクトの生成
		FILE* fp = nullptr;
		fopen_s(&fp, csoName, "rb");
		_ASSERT_EXPR_A(fp, "CSO File not found");

		fseek(fp, 0, SEEK_END);
		long csoSize{ ftell(fp) };
		fseek(fp, 0, SEEK_SET);

		std::unique_ptr<unsigned char[]> csoData{ std::make_unique<unsigned char[]>(csoSize) };
		fread(csoData.get(), csoSize, 1, fp);
		fclose(fp);

		HRESULT hr = device->CreateVertexShader(csoData.get(), csoSize, nullptr, vertexShader);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// 生成したデータをセット
		vertexShaderData.insert(std::make_pair(csoName, *vertexShader));

		// 入力レイアウトオブジェクトの生成
		if (inputLayout)
		{
			hr = device->CreateInputLayout(inputElementDesc, numElements,
				csoData.get(), csoSize, inputLayout);
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

			// 生成したデータをセット
			inputLayoutData.insert(std::make_pair(csoName, *inputLayout));
		}

		DebugSupporter::Instance().OutputString(L"頂点シェーダーの作成成功\n");
		DebugSupporter::Instance().OutputString("\t");
		DebugSupporter::Instance().OutputString(csoName);
		DebugSupporter::Instance().OutputString("\n");
	}
}

// ピクセルシェーダー読み込み
HRESULT GpuResourceManager::LoadPixelShader(
	ID3D11Device* device,
	const char* filename,
	ID3D11PixelShader** pixelShader)
{
	// ファイルを開く
	FILE* fp = nullptr;
	fopen_s(&fp, filename, "rb");
	_ASSERT_EXPR_A(fp, "Pixel Shader File not found");

	// ファイルのサイズを求める
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// メモリ上に頂点シェーダーデータを格納する領域を用意する
	std::unique_ptr<u_char[]> data = std::make_unique<u_char[]>(size);
	fread(data.get(), size, 1, fp);
	fclose(fp);

	// ピクセルシェーダー生成
	HRESULT hr = device->CreatePixelShader(data.get(), size, nullptr, pixelShader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return hr;
}

// ピクセルシェーダ作成
void GpuResourceManager::CreatePsFromCso(ID3D11Device* device,
	const char* csoName,
	ID3D11PixelShader** pixelShader)
{
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>> pixelShaderData;

	// 過去に生成しているか確認
	auto it = pixelShaderData.find(csoName);
	if (it != pixelShaderData.end())
	{
		// 過去に生成したデータを取得
		*pixelShader = it->second.Get();
		(*pixelShader)->AddRef();
	}
	else
	{
		// ピクセルシェーダーオブジェクトの生成
		FILE* fp = nullptr;
		fopen_s(&fp, csoName, "rb");
		_ASSERT_EXPR_A(fp, "CSO File not found");

		fseek(fp, 0, SEEK_END);
		long csoSize = { ftell(fp) };
		fseek(fp, 0, SEEK_SET);

		std::unique_ptr<unsigned char[]> csoData = { std::make_unique<unsigned char[]>(csoSize) };
		fread(csoData.get(), csoSize, 1, fp);
		fclose(fp);

		HRESULT hr = device->CreatePixelShader(csoData.get(), csoSize, nullptr, pixelShader);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// 生成したデータをセット
		pixelShaderData.insert(std::make_pair(csoName, *pixelShader));

		DebugSupporter::Instance().OutputString(L"ピクセルシェーダーの作成成功\n");
		DebugSupporter::Instance().OutputString("\t");
		DebugSupporter::Instance().OutputString(csoName);
		DebugSupporter::Instance().OutputString("\n");
	}
}

// ジオメトリシェーダー作成
void GpuResourceManager::CreateGsFromCso(ID3D11Device* device,
	const char* csoName, 
	ID3D11GeometryShader** geometryShader)
{
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11GeometryShader>> geometryShaderData;

	// 過去に生成しているか確認
	auto it = geometryShaderData.find(csoName);
	if (it != geometryShaderData.end())
	{
		// 過去に生成したデータを取得
		*geometryShader = it->second.Get();
		(*geometryShader)->AddRef();
	}
	else
	{
		FILE* fp{ nullptr };
		fopen_s(&fp, csoName, "rb");
		_ASSERT_EXPR_A(fp, "CSO File not found");

		fseek(fp, 0, SEEK_END);
		long cso_sz{ ftell(fp) };
		fseek(fp, 0, SEEK_SET);

		std::unique_ptr<unsigned char[]> cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
		fread(cso_data.get(), cso_sz, 1, fp);
		fclose(fp);

		HRESULT hr{ S_OK };
		hr = device->CreateGeometryShader(cso_data.get(), cso_sz, nullptr, geometryShader);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// 生成したデータをセット
		geometryShaderData.insert(std::make_pair(csoName, *geometryShader));

		DebugSupporter::Instance().OutputString(L"ジオメトリシェーダーの作成成功\n");
		DebugSupporter::Instance().OutputString("\t");
		DebugSupporter::Instance().OutputString(csoName);
		DebugSupporter::Instance().OutputString("\n");
	}
}

// コンピュートシェーダー作成
void GpuResourceManager::CreateCsFromCso(ID3D11Device* device,
	const char* csoName,
	ID3D11ComputeShader** computeShader)
{
	FILE* fp{ nullptr };
	fopen_s(&fp, csoName, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> csoData = std::make_unique<unsigned char[]>(cso_sz);
	fread(csoData.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr{ S_OK };
	hr = device->CreateComputeShader(csoData.get(), cso_sz, nullptr, computeShader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	DebugSupporter::Instance().OutputString(L"コンピュートシェーダーの作成成功\n");
	DebugSupporter::Instance().OutputString("\t");
	DebugSupporter::Instance().OutputString(csoName);
	DebugSupporter::Instance().OutputString("\n");
}

// テクスチャ読み込み
void GpuResourceManager::LoadTextureFromFile(ID3D11Device* device,
	const wchar_t* filename, 
	ID3D11ShaderResourceView** shaderResourceView,
	D3D11_TEXTURE2D_DESC* texture2dDesc)
{
	static std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> resources;

	HRESULT hr = { S_OK };
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;

	// DDSファイルがあるか確認
	std::filesystem::path ddsFilename(filename);
	ddsFilename.replace_extension("dds");
	if (std::filesystem::exists(ddsFilename.c_str()))
	{
		// 過去に読み込んだことがあるか確認
		auto it = resources.find(ddsFilename.c_str());
		if (it != resources.end())
		{
			*shaderResourceView = it->second.Get();
			(*shaderResourceView)->AddRef();
			(*shaderResourceView)->GetResource(resource.GetAddressOf());
		}
		else
		{
			hr = DirectX::CreateDDSTextureFromFile(device, ddsFilename.c_str(), resource.GetAddressOf(),
				shaderResourceView);
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

			resources.insert(std::make_pair(ddsFilename.c_str(), *shaderResourceView));
		}
	}
	else
	{
		// 過去に読み込んだことがあるか確認
		auto it = resources.find(filename);
		if (it != resources.end())
		{
			*shaderResourceView = it->second.Get();
			(*shaderResourceView)->AddRef();
			(*shaderResourceView)->GetResource(resource.GetAddressOf());
		}
		else
		{
			hr = DirectX::CreateWICTextureFromFile(device, filename, resource.GetAddressOf(),
				shaderResourceView);
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

			resources.insert(std::make_pair(filename, *shaderResourceView));
		}
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	texture2d->GetDesc(texture2dDesc);
}

// ダミーテクスチャ作成
void GpuResourceManager::MakeDummyTexture(ID3D11Device* device,
	ID3D11ShaderResourceView** shaderResouceView,
	DWORD value, 
	UINT dimension)
{
	HRESULT hr{ S_OK };

	D3D11_TEXTURE2D_DESC texture2dDesc{};
	texture2dDesc.Width = dimension;
	texture2dDesc.Height = dimension;
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	size_t texels = static_cast<size_t>(dimension) * dimension;
	std::unique_ptr<DWORD[]> sysmem{ std::make_unique<DWORD[]>(texels) };
	for (size_t i = 0; i < texels; ++i) sysmem[i] = value;

	D3D11_SUBRESOURCE_DATA subresource_data{};
	subresource_data.pSysMem = sysmem.get();
	subresource_data.SysMemPitch = sizeof(DWORD) * dimension;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	hr = device->CreateTexture2D(&texture2dDesc, &subresource_data, &texture2d);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr)); 

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
	shaderResourceViewDesc.Format = texture2dDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(texture2d.Get(), &shaderResourceViewDesc,
		shaderResouceView);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

void GpuResourceManager::MakeDummyTexture(ID3D11Device* device, 
	ID3D11ShaderResourceView** shaderResourceView,
	D3D11_TEXTURE2D_DESC* texture2dDesc,
	DWORD value, UINT dimension)
{
	HRESULT hr{ S_OK };

	texture2dDesc->Width = dimension;
	texture2dDesc->Height = dimension;
	texture2dDesc->MipLevels = 1;
	texture2dDesc->ArraySize = 1;
	texture2dDesc->Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture2dDesc->SampleDesc.Count = 1;
	texture2dDesc->SampleDesc.Quality = 0;
	texture2dDesc->Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc->BindFlags = D3D11_BIND_SHADER_RESOURCE;

	size_t texels = static_cast<size_t>(dimension) * dimension;
	std::unique_ptr<DWORD[]> sysmem{ std::make_unique<DWORD[]>(texels) };
	for (size_t i = 0; i < texels; ++i) sysmem[i] = value;

	D3D11_SUBRESOURCE_DATA subresource_data{};
	subresource_data.pSysMem = sysmem.get();
	subresource_data.SysMemPitch = sizeof(DWORD) * dimension;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	hr = device->CreateTexture2D(texture2dDesc, &subresource_data, &texture2d);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
	shaderResourceViewDesc.Format = texture2dDesc->Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(texture2d.Get(), &shaderResourceViewDesc,
		shaderResourceView);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

// 定数バッファ作成
HRESULT GpuResourceManager::CreateConstantBuffer(
	ID3D11Device* device,
	UINT bufferSize,
	ID3D11Buffer** constantBuffer)
{
	D3D11_BUFFER_DESC desc{};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.ByteWidth = bufferSize;
	desc.StructureByteStride = 0;

	HRESULT hr = device->CreateBuffer(&desc, 0, constantBuffer);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return hr;
}
