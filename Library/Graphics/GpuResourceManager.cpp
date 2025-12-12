#include "GpuResourceManager.h"
#include <filesystem>

#include <d3dcompiler.h>
#include <memory>
#include <fstream>

#include <imgui.h>

#include "../../External/DirectXTex/DDS.h"
#include "../../External/DirectXTK-main/WICTextureLoader.h"
#include "../../External/DirectXTK-main/DDSTextureLoader.h"
#include "../../External/DirectXTex/DirectXTex.h"
#include "../HRTrace.h"

#include "../DebugSupporter/DebugSupporter.h"

#pragma comment(lib, "d3dcompiler.lib")

std::map<std::string, Microsoft::WRL::ComPtr<ID3D11VertexShader>>	GpuResourceManager::vertexShaderMap;
std::map<std::string, Microsoft::WRL::ComPtr<ID3D11InputLayout>>	GpuResourceManager::inputLayoutMap;
std::map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>>	GpuResourceManager::pixelShaderMap;
std::map<std::string, Microsoft::WRL::ComPtr<ID3D11GeometryShader>> GpuResourceManager::geometryShaderMap;

std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> GpuResourceManager::resources;

bool GpuResourceManager::_isDrawShaderGui = false;

// ファイル階層を表すツリーノード
struct FileNode
{
	// 子ノード (次のパス要素 -> ノード)
	std::map<std::string, FileNode> children;

	// このノードがファイル（シェーダー）そのものを表す場合、
	// 元のフルパスを保持する
	std::string fullPath;
	bool isFile = false;
};
/**
 * @brief パス文字列をディレクトリ/ファイル名に分割する
 * @param path ファイルパス (例: "Data/Shader/Test.hlsl")
 * @return パスの各要素 (例: {"Data", "Shader", "Test.hlsl"})
 */
std::vector<std::string> SplitPath(const std::string& pathStr)
{
	std::vector<std::string> parts;
	// std::filesystem::path を使うと、/ と \ の両方に自動対応できる
	std::filesystem::path path(pathStr);

	for (const auto& part : path)
	{
		parts.push_back(part.string());
	}
	return parts;
}

/**
 * @brief シェーダーマップからファイルツリー構造を構築する
 * @param shaderMap シェーダーのマップ
 * @param prefixToRemove GUIで非表示にしたい先頭のプレフィックス (例: ".Data/Shader/HLSL")
 */
template<typename TShaderMap>
FileNode BuildFileTree(const TShaderMap& shaderMap, const std::vector<std::string>& prefixToRemoves = std::vector<std::string>())
{
	FileNode root;

	for (const auto& [filepathStr, shader] : shaderMap)
	{
		std::string displayPathStr = filepathStr;

		for (auto& prefixToRemove : prefixToRemoves)
		{
			size_t prefixLength = prefixToRemove.length();

			// C++17 での "starts_with" の代替
			if (!prefixToRemove.empty() && displayPathStr.rfind(prefixToRemove, 0) == 0)
			{
				size_t actualPrefixLength = prefixLength;
				// プレフィックスの直後のスラッシュも削除 (例: ".Data/Shader/HLSL/")
				if (displayPathStr.length() > prefixLength &&
					(displayPathStr[prefixLength] == '/' || displayPathStr[prefixLength] == '\\'))
				{
					actualPrefixLength++;
				}
				// プレフィックスを除いた部分パスを取得
				displayPathStr = displayPathStr.substr(actualPrefixLength);
			}
		}

		// 表示用パスを分割
		std::vector<std::string> parts = SplitPath(displayPathStr);
		FileNode* currentNode = &root;

		for (size_t i = 0; i < parts.size(); ++i)
		{
			const std::string& part = parts[i];
			if (part.empty()) continue; // 空のパス要素はスキップ

			currentNode = &currentNode->children[part];

			if (i == parts.size() - 1)
			{
				// ノードがファイルの場合
				currentNode->isFile = true;
				// ★再コンパイル用に元のフルパスを保存
				currentNode->fullPath = filepathStr;
			}
		}
	}
	return root;
}

/**
 * @brief ImGuiでツリーを再帰的に描画する
 * @param node 描画するツリーのノード
 * @param recompileFunc 再コンパイルボタンが押されたときに呼ぶ関数(const char* path)
 */
template<typename ReCompileFunc>
void RenderShaderTree(const FileNode& node, ReCompileFunc recompileFunc)
{
	// 子ノードをイテレート (std::mapなのでキーでソート済み)
	for (const auto& [name, childNode] : node.children)
	{
		if (childNode.isFile)
		{
			// ノードがファイルの場合:
			// ImGui::TreeNodeFlags_Leaf を指定して「葉」であることを示す
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			ImGui::TreeNodeEx(name.c_str(), flags);

			// ファイルノードの横にボタンを配置
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			{
				// (オプション) ダブルクリックで再コンパイル
				recompileFunc(childNode.fullPath.c_str());
			}

			// ボタンを同じ行に表示
			ImGui::SameLine();
			// IDが重複しないように、フルパスをIDとしてプッシュする
			ImGui::PushID(childNode.fullPath.c_str());
			if (ImGui::SmallButton(u8"再コンパイル"))
			{
				recompileFunc(childNode.fullPath.c_str());
			}
			ImGui::PopID();
		}
		else
		{
			// ノードがディレクトリの場合:
			if (ImGui::TreeNode(name.c_str())) // ディレクトリ名をノードとして
			{
				RenderShaderTree(childNode, recompileFunc); // 再帰呼び出し
				ImGui::TreePop();
			}
		}
	}
}

// Gui描画
void GpuResourceManager::DrawGui(ID3D11Device* device)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu(u8"デバッグ"))
		{
			ImGui::Checkbox(u8"シェーダリソース", &_isDrawShaderGui);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	const std::vector<std::string>& prefixToRemoves = { ".", "Data", "Shader", "HLSL" };

	if (_isDrawShaderGui)
	{
		if (ImGui::Begin(u8"シェーダリソース"))
		{
			if (ImGui::TreeNode(u8"頂点シェーダー"))
			{
				// 頂点シェーダーのツリーを構築
				FileNode vsTreeRoot = BuildFileTree(vertexShaderMap, prefixToRemoves);

				// ツリーを描画 (ラムダで再コンパイル処理を渡す)
				RenderShaderTree(vsTreeRoot, [&](const char* path) {
					GpuResourceManager::ReCompileVertexShader(device, path);
					});

				ImGui::TreePop();
			}

			if (ImGui::TreeNode(u8"ピクセルシェーダ"))
			{
				// ピクセルシェーダーのツリーを構築
				FileNode psTreeRoot = BuildFileTree(pixelShaderMap, prefixToRemoves);

				// ツリーを描画
				RenderShaderTree(psTreeRoot, [&](const char* path) {
					GpuResourceManager::ReCompilePixelShader(device, path);
					});

				ImGui::TreePop();
			}
		}
		ImGui::End();
	}
}

// 頂点シェーダー取得
Microsoft::WRL::ComPtr<ID3D11VertexShader>& GpuResourceManager::GetVertexShader(const std::string& filepath)
{
	return vertexShaderMap[filepath];
}
// 入力レイアウト取得
Microsoft::WRL::ComPtr<ID3D11InputLayout>& GpuResourceManager::GetInputLayout(const std::string& filepath)
{
	return inputLayoutMap[filepath];
}
// ピクセルシェーダ取得
Microsoft::WRL::ComPtr<ID3D11PixelShader>& GpuResourceManager::GetPixelShader(const std::string& filepath)
{
	return pixelShaderMap[filepath];
}

// ジオメトリシェーダ取得
Microsoft::WRL::ComPtr<ID3D11GeometryShader>& GpuResourceManager::GetGeometryShader(const std::string& filepath)
{
	return geometryShaderMap[filepath];
}

// 頂点シェーダ作成
void GpuResourceManager::CreateVsFromCso(ID3D11Device* device, 
	const char* csoName,
	ID3D11VertexShader** vertexShader, 
	ID3D11InputLayout** inputLayout, 
	D3D11_INPUT_ELEMENT_DESC* inputElementDesc, 
	UINT numElements)
{
	HRESULT hr = { S_OK };

	// 過去に生成しているか確認
	auto it = vertexShaderMap.find(csoName);
	if (it != vertexShaderMap.end())
	{
		// 過去に生成したデータを取得
		*vertexShader = it->second.Get();
		(*vertexShader)->AddRef();

		auto inputIter = inputLayoutMap.find(csoName);
		if (inputIter != inputLayoutMap.end())
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
		vertexShaderMap.insert(std::make_pair(csoName, *vertexShader));

		// 入力レイアウトオブジェクトの生成
		if (inputLayout && inputElementDesc)
		{
			hr = device->CreateInputLayout(inputElementDesc, numElements,
				csoData.get(), csoSize, inputLayout);
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

			// 生成したデータをセット
			inputLayoutMap.insert(std::make_pair(csoName, *inputLayout));
		}

		Debug::Output::String(L"頂点シェーダーの作成成功\n");
		Debug::Output::String("\t");
		Debug::Output::String(csoName);
		Debug::Output::String("\n");
	}
}

// ピクセルシェーダ作成
void GpuResourceManager::CreatePsFromCso(ID3D11Device* device,
	const char* csoName,
	ID3D11PixelShader** pixelShader)
{
	// 過去に生成しているか確認
	auto it = pixelShaderMap.find(csoName);
	if (it != pixelShaderMap.end())
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
		pixelShaderMap.insert(std::make_pair(csoName, *pixelShader));

		Debug::Output::String(L"ピクセルシェーダーの作成成功\n");
		Debug::Output::String("\t");
		Debug::Output::String(csoName);
		Debug::Output::String("\n");
	}
}

// ジオメトリシェーダー作成
void GpuResourceManager::CreateGsFromCso(ID3D11Device* device,
	const char* csoName, 
	ID3D11GeometryShader** geometryShader)
{
	// 過去に生成しているか確認
	auto it = geometryShaderMap.find(csoName);
	if (it != geometryShaderMap.end())
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
		geometryShaderMap.insert(std::make_pair(csoName, *geometryShader));

		Debug::Output::String(L"ジオメトリシェーダーの作成成功\n");
		Debug::Output::String("\t");
		Debug::Output::String(csoName);
		Debug::Output::String("\n");
	}
}

// ジオメトリシェーダー作成
void GpuResourceManager::CreateGsWithStreamOutFromCso(ID3D11Device* device, 
	const char* csoName, 
	ID3D11GeometryShader** geometryShader,
	const D3D11_SO_DECLARATION_ENTRY* declaration,
	UINT numEntries,
	const UINT* bufferStrides, 
	UINT numStrides,
	UINT rasterizedStream)
{
	FILE* fp{ nullptr };
	fopen_s(&fp, csoName, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> csoData{ std::make_unique<unsigned char[]>(cso_sz) };
	fread(csoData.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr{ S_OK };
	hr = device->CreateGeometryShaderWithStreamOutput(csoData.get(),
		cso_sz, 
		declaration,
		numEntries,
		bufferStrides,
		numStrides,
		rasterizedStream,
		nullptr, 
		geometryShader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
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

	Debug::Output::String(L"コンピュートシェーダーの作成成功\n");
	Debug::Output::String("\t");
	Debug::Output::String(csoName);
	Debug::Output::String("\n");
}

// ドメインシェーダー作成
void GpuResourceManager::CreateDsFromCso(ID3D11Device* device,
	const char* csoName, 
	ID3D11DomainShader** domainShader)
{
	FILE* fp = nullptr;
	fopen_s(&fp, csoName, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateDomainShader(cso_data.get(), cso_sz, nullptr, domainShader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	Debug::Output::String(L"ドメインシェーダーの作成成功\n");
	Debug::Output::String("\t");
	Debug::Output::String(csoName);
	Debug::Output::String("\n");
}

// ハルシェーダー作成
void GpuResourceManager::CreateHsFromCso(ID3D11Device* device,
	const char* csoName, 
	ID3D11HullShader** hullShader)
{
	FILE* fp = nullptr;
	fopen_s(&fp, csoName, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateHullShader(cso_data.get(), cso_sz, nullptr, hullShader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	Debug::Output::String(L"ハルシェーダーの作成成功\n");
	Debug::Output::String("\t");
	Debug::Output::String(csoName);
	Debug::Output::String("\n");
}

// テクスチャ読み込み
bool GpuResourceManager::LoadTextureFromFile(ID3D11Device* device,
	const wchar_t* filename, 
	ID3D11ShaderResourceView** shaderResourceView,
	D3D11_TEXTURE2D_DESC* texture2dDesc)
{
	HRESULT hr = { S_OK };
	std::filesystem::path filepath(filename);

	// DDSファイルが存在するか確認
	{
		std::filesystem::path ddsPath = filepath;
		ddsPath.replace_extension(".dds");
		if (std::filesystem::exists(ddsPath))
		{
			// DDSファイルが存在したらそちらを読み込む
			filepath = ddsPath;
		}
	}

	// 以前に読み込んだことがあるか確認
	auto it = resources.find(filepath.wstring());
	if (it != resources.end())
	{
		// 過去に読み込んだデータを取得
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		*shaderResourceView = it->second.Get();
		(*shaderResourceView)->AddRef();
		(*shaderResourceView)->GetResource(resource.GetAddressOf());
		if (texture2dDesc)
		{
			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
			resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
			texture2d->GetDesc(texture2dDesc);
		}
		return true;
	}

	// ファイル拡張子を確認してそれに応じた読み込みを行う
	std::string extension = filepath.extension().string();
	// 小文字化
	std::transform(extension.begin(), extension.end(), extension.begin(), tolower);

	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratch_image;
	if (extension == ".dds")
	{
		hr = DirectX::GetMetadataFromDDSFile(filepath.wstring().c_str(), DirectX::DDS_FLAGS_NONE, metadata);
		// 失敗したらfalse
		if (!SUCCEEDED(hr))
			return false;
		hr = DirectX::LoadFromDDSFile(filepath.wstring().c_str(), DirectX::DDS_FLAGS_NONE, &metadata, scratch_image);
		// 失敗したらfalse
		if (!SUCCEEDED(hr))
			return false;
	}
	else if (extension == ".tga")
	{
		hr = DirectX::GetMetadataFromTGAFile(filepath.wstring().c_str(), metadata);
		// 失敗したらfalse
		if (!SUCCEEDED(hr))
			return false;
		hr = DirectX::LoadFromTGAFile(filepath.wstring().c_str(), &metadata, scratch_image);
		// 失敗したらfalse
		if (!SUCCEEDED(hr))
			return false;
	}
	else if (extension == ".hdr")
	{
		hr = DirectX::GetMetadataFromHDRFile(filepath.wstring().c_str(), metadata);
		// 失敗したらfalse
		if (!SUCCEEDED(hr))
			return false;
		hr = DirectX::LoadFromHDRFile(filepath.wstring().c_str(), &metadata, scratch_image);
		// 失敗したらfalse
		if (!SUCCEEDED(hr))
			return false;
	}
	else
	{
		hr = DirectX::GetMetadataFromWICFile(filepath.wstring().c_str(), DirectX::WIC_FLAGS_NONE, metadata);
		// 失敗したらfalse
		if (!SUCCEEDED(hr))
			return false;
		hr = DirectX::LoadFromWICFile(filepath.wstring().c_str(), DirectX::WIC_FLAGS_NONE, &metadata, scratch_image);
		// 失敗したらfalse
		if (!SUCCEEDED(hr))
			return false;
	}

	// シェーダーリソースビュー作成
	hr = DirectX::CreateShaderResourceView(device,
		scratch_image.GetImages(), 
		scratch_image.GetImageCount(),
		metadata, shaderResourceView);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// 生成したデータをセット
	resources.insert(std::make_pair(filepath.wstring(), *shaderResourceView));

	if (texture2dDesc)
	{
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		(*shaderResourceView)->GetResource(resource.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		texture2d->GetDesc(texture2dDesc);
	}

	Debug::Output::String(L"テクスチャ読み込み成功\n");
	Debug::Output::String("\t");
	Debug::Output::String(filepath.wstring());
	Debug::Output::String("\n");

	return true;
}

// MipMapテクスチャ読み込み
bool GpuResourceManager::LoadMipMapTextureFromFile(ID3D11Device* device,
	const wchar_t* filename,
	ID3D11ShaderResourceView** shaderResourceView)
{
	DirectX::ScratchImage scratchImage;
	DirectX::TexMetadata metadata;
	HRESULT hr;

	// DDSファイルを ScratchImage に読み込む
	hr = DirectX::LoadFromDDSFile(
		filename,
		DirectX::DDS_FLAGS_NONE,
		&metadata,
		scratchImage
	);

	if (FAILED(hr))
	{
		// DDSの読み込みに失敗
		return false;
	}

	// ミップマップが1レベル（存在しない）場合のみ、ミップマップを生成する
	if (metadata.mipLevels == 1)
	{
		DirectX::ScratchImage mipChain;

		// GenerateMipMaps を呼び出してミップマップチェーンを生成
		hr = DirectX::GenerateMipMaps(
			scratchImage.GetImages(),
			scratchImage.GetImageCount(),
			metadata,
			DirectX::TEX_FILTER_DEFAULT, // デフォルトのフィルター (ボックスフィルター)
			0,                           // 生成するレベル数 (0 = フルチェーン)
			mipChain                     // 結果を格納する ScratchImage
		);

		if (FAILED(hr))
		{
			// ミップマップ生成失敗
			return false;
		}

		// 元のイメージをミップマップ付きのイメージで置き換える
		scratchImage = std::move(mipChain);
		metadata = scratchImage.GetMetadata();
	}

	// D3Dリソースを作成
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	hr = DirectX::CreateTexture(
		device,
		scratchImage.GetImages(),
		scratchImage.GetImageCount(),
		metadata,
		resource.GetAddressOf()
	);

	if (FAILED(hr))
	{
		// テクスチャリソース作成失敗
		return false;
	}

	// SRV (Shader Resource View) を作成
	hr = device->CreateShaderResourceView(
		resource.Get(),
		nullptr, // デフォルトのビューデスク
		shaderResourceView    // 結果のSRV
	);

	return true;
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

// SRVのコピーを作成
void GpuResourceManager::CreateShaderResourceViewCopy(
	ID3D11Device* device,
	ID3D11DeviceContext* dc, 
	ID3D11ShaderResourceView* sourceSRV, 
	D3D11_SHADER_RESOURCE_VIEW_DESC* srvDesc, 
	ID3D11ShaderResourceView** destSRV)
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D11Resource> pSourceResource;
	sourceSRV->GetResource(pSourceResource.GetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pSourceTexture;
	hr = pSourceResource.As(&pSourceTexture);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	D3D11_TEXTURE2D_DESC desc;
	pSourceTexture->GetDesc(&desc);
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDestTexture;
	hr = device->CreateTexture2D(&desc, nullptr, pDestTexture.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	dc->CopyResource(pDestTexture.Get(), pSourceTexture.Get());
	hr = device->CreateShaderResourceView(pDestTexture.Get(),
		srvDesc, destSRV);
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


Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  cachedDSS;
Microsoft::WRL::ComPtr<ID3D11RasterizerState>  cachedRS;
Microsoft::WRL::ComPtr<ID3D11BlendState>  cachedBS;
FLOAT blendFactor[4];
UINT sampleMask;
// 現在使用しているステートのキャッシュを保存
void GpuResourceManager::SaveStateCache(ID3D11DeviceContext* dc)
{
	dc->OMGetDepthStencilState(cachedDSS.GetAddressOf(), 0);
	dc->RSGetState(cachedRS.GetAddressOf());
	dc->OMGetBlendState(cachedBS.GetAddressOf(), blendFactor, &sampleMask);
}
// 保存してあるステートのキャッシュを復元
void GpuResourceManager::RestoreStateCache(ID3D11DeviceContext* dc)
{
	dc->OMSetDepthStencilState(cachedDSS.Get(), 0);
	dc->RSSetState(cachedRS.Get());
	dc->OMSetBlendState(cachedBS.Get(), blendFactor, sampleMask);

    cachedDSS.Reset();
    cachedRS.Reset();
    cachedBS.Reset();
}
// ID3DIncludeインターフェースを実装するカスタムクラス
class ShaderIncludeHandler : public ID3DInclude {
public:
	// ベースとなるディレクトリ（検索の基点）を保持する
	std::filesystem::path m_baseDirectory;

	ShaderIncludeHandler(const std::filesystem::path& baseDir)
		: m_baseDirectory(baseDir) {
	}

	// #include が見つかったときに呼び出される
	HRESULT STDMETHODCALLTYPE Open(
		D3D_INCLUDE_TYPE IncludeType, // インクルードのタイプ (ローカルかシステムか)
		LPCSTR           pFileName,   // インクルードするファイル名 (例: "../Define/SamplerStateDefine.hlsli")
		LPCVOID          pParentData, // 親ファイルデータ (今回は使わない)
		LPCVOID* ppData,      // [out] 読み込んだファイルデータへのポインタを格納する
		UINT* pBytes)       // [out] 読み込んだデータのバイト数を格納する
	{
		// pFileName は相対パス (例: "../Define/SamplerStateDefine.hlsli")
		// ベースディレクトリと組み合わせてフルパスを作成する

		// std::filesystem を使ってパスを正規化・結合する
		// (m_baseDirectory / pFileName) でパスを結合し、canonicalで絶対パスに解決
		try {
			std::filesystem::path filePath = std::filesystem::canonical(m_baseDirectory / pFileName);

			if (!std::filesystem::exists(filePath)) {
				// ファイルが見つからない
				return E_FAIL;
			}

			// ファイルをバイナリとして読み込む
			std::ifstream file(filePath, std::ios::binary | std::ios::ate);
			if (!file.is_open()) {
				return E_FAIL;
			}

			std::streamsize size = file.tellg();
			file.seekg(0, std::ios::beg);

			// 読み込んだデータを格納するバッファをヒープに確保
			// (このメモリは Close() メソッドで解放される)
			char* buffer = new char[size];
			if (!file.read(buffer, size)) {
				delete[] buffer; // 読み込み失敗
				return E_FAIL;
			}

			// 出力ポインタに設定
			*ppData = buffer;
			*pBytes = static_cast<UINT>(size);

			return S_OK;
		}
		catch (const std::filesystem::filesystem_error& e) {
			// パス解決エラーなど
			OutputDebugStringA(e.what());
			return E_FAIL;
		}
	}

	// Open() で読み込みが終わった後に呼び出される
	HRESULT STDMETHODCALLTYPE Close(LPCVOID pData) // Open() で *ppData に設定したポインタ
	{
		// Open() で確保したメモリを解放する
		char* buffer = (char*)pData;
		delete[] buffer;
		return S_OK;
	}
};

// 頂点シェーダーを再コンパイルする関数
bool GpuResourceManager::ReCompileVertexShader(
	ID3D11Device* device, 
	const std::string& filepath)
{
	Microsoft::WRL::ComPtr<ID3D11VertexShader>& vertexShader = vertexShaderMap[filepath];
	// まだコンパイルされていない場合は、falseを返す
	if (!vertexShader)
		return false;

	UINT compileFlags = 0;
#if defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	std::string replacePath = filepath;
	// ファイル階層がCSOとhlslでは異なるので"Data/Shader"を取り除く
	size_t pos = replacePath.find("Data/Shader");
	if (pos != std::string::npos)
	{
		replacePath.erase(pos, 12); // "Data/Shader"の長さは12
	}
	std::filesystem::path path(replacePath);
	path.replace_extension(".hlsl");
	Microsoft::WRL::ComPtr<ID3DBlob> outBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

	// shaderPath (例: "Data/Shader/PrimitiveRendererPS.hlsl") の
	// 親ディレクトリ (例: "Data/Shader/") を取得する
	std::filesystem::path baseDir = std::filesystem::path(replacePath).parent_path();

	// カスタムハンドラを作成
	std::unique_ptr<ShaderIncludeHandler> includeHandler =
		std::make_unique<ShaderIncludeHandler>(baseDir);

	HRESULT hr = D3DCompileFromFile(
		path.c_str(),
		nullptr,
		includeHandler.get(),
		"main",
		"vs_5_0",
		compileFlags,
		0,
		outBlob.ReleaseAndGetAddressOf(),
		errorBlob.ReleaseAndGetAddressOf()
	);

	// シェーダーの再コンパイルに失敗した場合
	if (FAILED(hr)) 
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}
		return false;
	}

	// 再コンパイル
	hr = device->CreateVertexShader(
		outBlob->GetBufferPointer(),
		outBlob->GetBufferSize(),
		nullptr,
		vertexShader.ReleaseAndGetAddressOf()
	);

	if (FAILED(hr))
	{
		return false;
	}

	Debug::Output::String(L"頂点シェーダーの再コンパイル成功:");
	Debug::Output::String(replacePath.c_str());
	Debug::Output::String("\n");
	return true;
}

// ピクセルシェーダーを再コンパイルする関数
bool GpuResourceManager::ReCompilePixelShader(
	ID3D11Device* device,
	const std::string& filepath)
{
	Microsoft::WRL::ComPtr<ID3D11PixelShader>& pixelShader = pixelShaderMap[filepath];
	// まだコンパイルされていない場合は、falseを返す
	if (!pixelShader)
		return false;

	UINT compileFlags = 0;
#if defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	std::string replacePath = filepath;
	// ファイル階層がCSOとhlslでは異なるので"Data/Shader"を取り除く
	size_t pos = replacePath.find("Data/Shader");
	if (pos != std::string::npos)
	{
		replacePath.erase(pos, 12); // "Data/Shader"の長さは12
	}
	std::filesystem::path path(replacePath);
	path.replace_extension(".hlsl");
	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

	// shaderPath (例: "Data/Shader/PrimitiveRendererPS.hlsl") の
	// 親ディレクトリ (例: "Data/Shader/") を取得する
	std::filesystem::path baseDir = std::filesystem::path(replacePath).parent_path();

	// カスタムハンドラを作成
	std::unique_ptr<ShaderIncludeHandler> includeHandler =
		std::make_unique<ShaderIncludeHandler>(baseDir);

	HRESULT hr = D3DCompileFromFile(
		path.c_str(),
		nullptr,
		includeHandler.get(),
		"main",
		"ps_5_0",
		compileFlags,
		0,
		&shaderBlob,
		&errorBlob
	);

	if (FAILED(hr)) 
	{
		if (errorBlob) 
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}
		return false;
	}

	hr = device->CreatePixelShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		nullptr,
		pixelShader.ReleaseAndGetAddressOf()
	);

	if (FAILED(hr)) 
	{
		return false;
	}

	Debug::Output::String(L"ピクセルシェーダーの再コンパイル成功:");
	Debug::Output::String(replacePath.c_str());
	Debug::Output::String("\n");
	return true;
}
