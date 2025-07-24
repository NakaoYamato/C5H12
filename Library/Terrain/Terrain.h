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

#include "TerrainTransparentWall.h"
#include "TerrainObjectLayout.h"

class Terrain
{
public:
    // ストリームアウト用
    struct StreamOutVertex
    {
        DirectX::XMFLOAT4 position = {};
        DirectX::XMFLOAT3 worldPosition = {};
        DirectX::XMFLOAT3 normal = {};
        DirectX::XMFLOAT2 texcoord = {};
		float             cost = 0.0f;
    };

	static constexpr size_t BaseColorTextureIndex = 0;
	static constexpr size_t NormalTextureIndex = 1;
    static constexpr LONG StreamOutMaxVertex = 3 * 3 * 128 * 128;
    static constexpr LONG MaterialMapSize = 1024 * 6;
    static constexpr LONG ParameterMapSize = 1024 * 3;
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
    ID3D11Buffer* const* GetAddressOfVertexBuffer() const { return _vertexBuffer.GetAddressOf(); }
	// インデックスバッファを取得
	ID3D11Buffer* GetIndexBuffer() const { return _indexBuffer.Get(); }
    // マテリアルマップのフレームバッファを取得
	FrameBuffer* GetMaterialMapFB() { return _materialMapFB.get(); }
    // パラメータマップのフレームバッファを取得
    FrameBuffer* GetParameterMapFB() { return _parameterMapFB.get(); }
    // ストリームアウトデータを取得
    const std::vector<StreamOutVertex>& GetStreamOutData() const { return _streamOutData; }
	// ストリームアウトデータを設定
	void SetStreamOutData(const std::vector<StreamOutVertex>& data) { _streamOutData = data; }
    // 透明壁取得
	 TerrainTransparentWall* GetTransparentWall() { return &_transparentWall; }
	// オブジェクトの配置情報を取得
	TerrainObjectLayout* GetTerrainObjectLayout() { return &_terrainObjectLayout; }

    // 書き出し
    void SaveToFile(const std::string& path);
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

    // ロード用SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _loadBaseColorSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _loadNormalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _loadParameterSRV;
#pragma endregion
    // マテリアルマップ(RT0:BaseColor,RT1:Normal)
    std::unique_ptr<FrameBuffer> _materialMapFB;
    // パラメータマップ（R：高さ、G：コスト、B：（空き）、A：（空き））
    std::unique_ptr<FrameBuffer> _parameterMapFB;
    // マテリアルのリセット
    bool _resetMap = false;
    // テクスチャをロードしたかどうか
    bool _isLoadingTextures = false;

    // ストリームアウト用
    std::vector<StreamOutVertex> _streamOutData;

    // 透明壁
	TerrainTransparentWall _transparentWall;

	// 環境オブジェクトの配置情報
	TerrainObjectLayout _terrainObjectLayout;

    // シリアライズパス
    std::string _serializePath;
    // 基本色テクスチャのパス
    std::wstring _baseColorTexturePath = L"";
    // 法線テクスチャのパス
    std::wstring _normalTexturePath = L"";
    // パラメータテクスチャのパス
    std::wstring _parameterTexturePath = L"";
};