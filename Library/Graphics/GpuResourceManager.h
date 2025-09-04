#pragma once

#include <d3d11.h>
#include <string>
#include <wrl.h>
#include <map>

class GpuResourceManager
{
public:
	// Gui描画
	static void DrawGui(ID3D11Device* device);

	// 頂点シェーダー取得
	static Microsoft::WRL::ComPtr<ID3D11VertexShader>& GetVertexShader(const std::string& filepath);
	// 入力レイアウト取得
	static Microsoft::WRL::ComPtr<ID3D11InputLayout>& GetInputLayout(const std::string& filepath);
	// ピクセルシェーダ取得
	static Microsoft::WRL::ComPtr<ID3D11PixelShader>& GetPixelShader(const std::string& filepath);
public:
	// 頂点シェーダ作成
	static void CreateVsFromCso(ID3D11Device* device,
		const char* csoName, 
		ID3D11VertexShader** vertexShader,
		ID3D11InputLayout** inputLayout, 
		D3D11_INPUT_ELEMENT_DESC* inputElementDesc,
		UINT numElements);

	// ピクセルシェーダ作成
	static void CreatePsFromCso(ID3D11Device* device,
		const char* csoName, 
		ID3D11PixelShader** pixelShader);

	// ジオメトリシェーダー作成
	static void CreateGsFromCso(ID3D11Device* device,
		const char* csoName, 
		ID3D11GeometryShader** geometryShader);

    // ジオメトリシェーダー作成
    static void CreateGsWithStreamOutFromCso(ID3D11Device* device,
        const char* csoName,
        ID3D11GeometryShader** geometryShader,
        const D3D11_SO_DECLARATION_ENTRY* declaration,
        UINT numEntries,
        const UINT* bufferStrides,
        UINT numStrides,
        UINT rasterizedStream);

	// コンピュートシェーダー作成
	static void CreateCsFromCso(ID3D11Device* device,
		const char* csoName,
		ID3D11ComputeShader** computeShader);

	// ドメインシェーダー作成
	static void CreateDsFromCso(ID3D11Device* device,
		const char* csoName,
		ID3D11DomainShader** domainShader);

	// ハルシェーダー作成
	static void CreateHsFromCso(ID3D11Device* device,
		const char* csoName,
		ID3D11HullShader** hullShader);

	// テクスチャ読み込み
	// 失敗でfalse
	static bool LoadTextureFromFile(ID3D11Device* device, const wchar_t* filename,
		ID3D11ShaderResourceView** shaderResourceView, D3D11_TEXTURE2D_DESC* texture2dDesc);

	// ダミーテクスチャ作成
	static void MakeDummyTexture(
		ID3D11Device* device,
		ID3D11ShaderResourceView** shaderResourceView,
		DWORD value/*0xAABBFFRR*/, UINT dimension);
	// ダミーテクスチャ作成
	static void MakeDummyTexture(
		ID3D11Device* device,
		ID3D11ShaderResourceView** shaderResourceView,
		D3D11_TEXTURE2D_DESC* texture2dDesc,
		DWORD value/*0xAABBFFRR*/, UINT dimension);

	// SRVのコピーを作成
	static void CreateShaderResourceViewCopy(
		ID3D11Device* device,
		ID3D11DeviceContext* dc,
		ID3D11ShaderResourceView* sourceSRV,
		D3D11_SHADER_RESOURCE_VIEW_DESC* srvDesc,
		ID3D11ShaderResourceView** destSRV);

	// 定数バッファ作成
	static HRESULT CreateConstantBuffer(
		ID3D11Device* device,
		UINT bufferSize,
		ID3D11Buffer** constantBuffer);

	// 現在使用しているステートのキャッシュを保存
    static void SaveStateCache(ID3D11DeviceContext* dc);

    // 保存してあるステートのキャッシュを復元
    static void RestoreStateCache(ID3D11DeviceContext* dc);

#pragma region 再コンパイル
	// 頂点シェーダーを再コンパイルする関数
	static bool ReCompileVertexShader(
		ID3D11Device* device,
		const std::string& filepath);

	// ピクセルシェーダーを再コンパイルする関数
	static bool ReCompilePixelShader(
		ID3D11Device* device,
		const std::string& filepath);
#pragma endregion

private:
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11VertexShader>> vertexShaderMap;
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11InputLayout>> inputLayoutMap;
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>> pixelShaderMap;
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11GeometryShader>> geometryShaderMap;
	static std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> resources;

	static bool _isDrawShaderGui;
};