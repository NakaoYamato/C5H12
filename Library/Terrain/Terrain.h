#pragma once

#include <d3d11.h>
#include <memory>
#include <wrl.h>
#include <string>
#include <unordered_map>

#include "../../Library/Math/Vector.h"
#include "../../Library/Model/Model.h"
#include "../../Library/Graphics/RenderContext.h"
#include "../../Library/Renderer/TextureRenderer.h"
#include "../../Library/PostProcess/FrameBuffer.h"

class Terrain
{
public:
    // ストリームアウト用
    struct StreamOutVertex
    {
        Vector4 position = {};
        Vector3 worldPosition = {};
        Vector3 worldNormal = {};
        Vector4 worldTangent = {};
        Vector4 parameter = {};
        Vector2 texcoord = {};
    };
    // コリジョン用ストリームアウト
	struct CollisionStreamOutVertex
	{
		Vector3 worldPosition = {};
		float padding{};
	};
    // テクスチャ品質
	enum class TextureQuality
	{
		Low     = 0,
		Medium  = 1,
		High    = 2,

		Max
	};

	static constexpr size_t BaseColorTextureIndex   = 0;
	static constexpr size_t NormalTextureIndex      = 1;
	static constexpr size_t ParameterTextureIndex   = 2;
    static LONG   MaterialMapSize[static_cast<size_t>(TextureQuality::Max)];
public:
    Terrain(ID3D11Device* device, const std::string& serializePath = "./Data/Terrain/Save/Test000.json");
	~Terrain() {}

    // テクスチャ更新
    // 編集を行うとtrueを返す
	bool UpdateTextures(TextureRenderer& textureRenderer, ID3D11DeviceContext* dc);
    // GUI描画
    void DrawGui(ID3D11Device* device, ID3D11DeviceContext* dc);

    // レイキャスト
	bool Raycast(
        const DirectX::XMFLOAT4X4& world,
        const Vector3& rayStart,
		const Vector3& rayDirection,
		float rayLength,
		Vector3* intersectionWorldPoint = nullptr,
		Vector3* intersectionWorldNormal = nullptr,
		Vector2* intersectUVPosition = nullptr) const;
#pragma region アクセサ
	// 頂点バッファを取得
    Microsoft::WRL::ComPtr<ID3D11Buffer>& GetVertexBuffer() { return _vertexBuffer; }
	// インデックスバッファを取得
    Microsoft::WRL::ComPtr<ID3D11Buffer>& GetIndexBuffer() { return _indexBuffer; }
	// 頂点情報をGPUに送るためのバッファを取得
	Microsoft::WRL::ComPtr<ID3D11Buffer>& GetStreamOutVertexBuffer() { return _streamOutVertexBuffer; }
	// コリジョン用ストリームアウトバッファを取得
	Microsoft::WRL::ComPtr<ID3D11Buffer>& GetCollisionStreamOutVertexBuffer() { return _collisionStreamOutVertexBuffer; }
	// コリジョン用ストリームアウトSRVを取得
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetCollisionStreamOutSRV() { return _collisionStreamOutSRV; }

    // マテリアルマップのフレームバッファを取得
	FrameBuffer* GetMaterialMapFB(TextureQuality quality = TextureQuality::High) { return _materialMapFB[static_cast<size_t>(quality)].get(); }
    
	// コリジョン用ストリームアウトデータを取得
	const std::vector<CollisionStreamOutVertex>& GetCollisionStreamOutData() const { return _collisionStreamOutData; }
	// コリジョン用ストリームアウトデータを設定
	void SetCollisionStreamOutData(ID3D11DeviceContext* dc, const std::vector<CollisionStreamOutVertex>& data);

	// Mipmap用SRVを取得
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetMipmapBaseColorSRV() { return _mipmapBaseColorSRV; }

    // 書き出し
    void SaveToFile(ID3D11Device* device, ID3D11DeviceContext* dc, const std::string& path);
    // 読み込み
    void LoadFromFile(ID3D11Device* device, const std::string& path);
#pragma endregion

private:
    // 地形メッシュの頂点とインデックスを生成
    void CreateTerrainMesh(ID3D11Device* device);

    // 基本色テクスチャのの書き出し
    void SaveBaseColorTexture(ID3D11Device* device, ID3D11DeviceContext* dc, const wchar_t* baseColorPath);
    // 法線テクスチャの書き出し
    void SaveNormalTexture(ID3D11Device* device, ID3D11DeviceContext* dc, const wchar_t* normalPath);
    // パラメータマップの書き出し
    void SaveParameterMap(ID3D11Device* device, ID3D11DeviceContext* dc, const wchar_t* heightMapPath);

private:
#pragma region 描画用COMオブジェクト
    Microsoft::WRL::ComPtr<ID3D11Buffer>	_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>    _indexBuffer;
    // 頂点情報をGPUに送るためのバッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> _streamOutVertexBuffer;
	// コリジョン用ストリームアウトバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> _collisionStreamOutVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _collisionStreamOutSRV;

    // ロード用SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _loadBaseColorSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _loadNormalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _loadParameterSRV;

	// Mipmap用SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _mipmapBaseColorSRV;
#pragma endregion
    // マテリアルマップ(RT0:BaseColor,RT1:Normal,RT2:Parameter{R：高さ、G：草、B：テクスチャのハイトマップ})
    std::unique_ptr<FrameBuffer> _materialMapFB[static_cast<size_t>(TextureQuality::Max)];
    // マテリアルのリセット
    bool _resetMap = false;
    // テクスチャをロードしたかどうか
    bool _isLoadingTextures = false;

	// コリジョン用頂点情報
	std::vector<CollisionStreamOutVertex> _collisionStreamOutData;

    // シリアライズパス
    std::string _serializePath;
    // 基本色テクスチャのパス
    std::wstring _baseColorTexturePath = L"";
    // 法線テクスチャのパス
    std::wstring _normalTexturePath = L"";
    // パラメータテクスチャのパス
    std::wstring _parameterTexturePath = L"";
};