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
#include "../../Library/Exporter/Exporter.h"

#include "TerrainTransparentWall.h"
#include "TerrainObjectLayout.h"

class Terrain
{
public:
	// 頂点データ構造体
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT2 texcoord;
    };
	// 定数バッファのデータ構造体
    struct ConstantBuffer
    {
        DirectX::XMFLOAT4X4 world = {};

        float edgeFactor    = 64.0f;    // エッジ分割数
        float innerFactor   = 64.0f;    // 内部部分数
        float heightScaler  = 1.0f;     // 高さ係数
        float padding0      = 0.0f;     // パディング

        float emissive      = 0.0f;     // エミッシブ
        float metalness     = 0.63f;    // メタリック
        float roughness     = 0.6f;     // ラフネス
        float padding1      = 0.0f;     // パディング
    };
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
    static constexpr LONG StreamOutMaxVertex = 3 * 3 * 64 * 64;
    static constexpr LONG MaterialMapSize = 2048;
    static constexpr LONG ParameterMapSize = 1024;
    static constexpr UINT ParameterMapIndex = 6;
public:
    Terrain(ID3D11Device* device, const std::string& serializePath = "./Data/Terrain/Save/Test000.json");
	~Terrain() {}
    // 描画処理
    void Render(TextureRenderer& textureRenderer, const RenderContext& rc, const DirectX::XMFLOAT4X4& world, bool writeGBuffer);
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
    // マテリアルマップのフレームバッファを取得
	FrameBuffer* GetMaterialMapFB() { return _materialMapFB.get(); }
    // パラメータマップのフレームバッファを取得
    FrameBuffer* GetParameterMapFB() { return _parameterMapFB.get(); }
    // ストリームアウトデータを取得
    const std::vector<StreamOutVertex>& GetStreamOutData() const { return _streamOutData; }
    // ストリームアウトをするかどうか設定
    void SetStreamOut(bool streamOut) { _streamOut = streamOut; }
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
    Microsoft::WRL::ComPtr<ID3D11Buffer>    _constantBuffer;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>	_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>	_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11HullShader>	_hullShader;
    Microsoft::WRL::ComPtr<ID3D11DomainShader>	_domainShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>	_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>	_gbPixelShader;

    // ストリームアウト用
    Microsoft::WRL::ComPtr<ID3D11Buffer> _streamOutVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> _streamOutCopyBuffer;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>	_streamOutGeometryShader;

    // ロード用SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _loadBaseColorSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _loadNormalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _loadParameterSRV;
#pragma endregion
    ConstantBuffer                          _data;
    // マテリアルマップ(RT0:BaseColor,RT1:Normal)
    std::unique_ptr<FrameBuffer> _materialMapFB;
    // パラメータマップ（R：高さ、G：コスト、B：（空き）、A：（空き））
    std::unique_ptr<FrameBuffer> _parameterMapFB;
    // マテリアルのリセット
    bool _resetMap = false;
    // テクスチャをロードしたかどうか
    bool _isLoadingTextures = false;

    // ストリームアウト用
    bool _streamOut = false;
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