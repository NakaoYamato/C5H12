#pragma once

#include <vector>
#include <mutex>

#include "../../Library/Terrain/Terrain.h"
#include "../../Library/Graphics/Shader.h"

class TerrainRenderer
{
public:
    // 頂点データ構造体
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
    };
    // 定数バッファのデータ構造体
    struct ConstantBuffer
    {
        DirectX::XMFLOAT4X4 world = {};

        Vector4 lodTessFactors = { 49.0f, 31.0f, 21.0f, 11.0f }; // LODの分割数

        int divisionCount = 15;             // 分割数
		float terrainLength = 50.0f;        // 地形1枚の長さ(m)
        float lodTessDistance = 15.0f;      // LODの距離
        float collisionTessFactor = 21.0f;  // 衝突判定用エッジ分割数(奇数のみ)

        float emissive = 0.0f;              // エミッシブ
        float metalness = 0.0f;            // メタリック
        float roughness = 1.0f;             // ラフネス
        float padding{};
    };
	struct GrassConstantBuffer
	{
        float grassTessellation = 8.0f;     // 草の分割数
        float lodDistanceMax = 100.0f;    // LOD距離
        float height = 1.0f;
        float width = 0.04f;

        float witherdFactor = 0.194f;
        float curvature = 0.6f;
        float heightVariance = 0.165f;
		float parlinNoiseDistribution = 0.178f;

		Vector4 specularColor{ 0.885f, 0.673f, 0.328f, 1.000f }; // 草のスペキュラカラー
	};
    // 描画用情報
    struct DrawInfo
    {
		Terrain* terrain = nullptr;  // 描画するTerrain
		DirectX::XMFLOAT4X4 world = {}; // ワールド行列
    };
    
	static constexpr float MaxTessellation = 64.0f;
    // 分割数
    static const size_t DivisionCount;
    // Terrain1枚の長さ(m)
    static const float TerrainLength;
public:
	TerrainRenderer() = default;
	~TerrainRenderer() = default;

	// 初期化
	void Initialize(ID3D11Device* device);
    // 頂点情報書き出し登録
    void ExportVertices(Terrain* terrain, const DirectX::XMFLOAT4X4& world);
    // 描画登録
	void Draw(Terrain* terrain, const DirectX::XMFLOAT4X4& world);
    // 影描画登録
    void DrawShadow(Terrain* terrain, const DirectX::XMFLOAT4X4& world);
    // 頂点書き出し処理
    void ExportVertex(const RenderContext& rc);
	// 描画処理
	void Render(const RenderContext& rc, bool writeGBuffer);
    // 影描画実行
    void CastShadow(const RenderContext& rc);
	// GUI描画
	void DrawGui();

	// テクスチャ品質取得
	Terrain::TextureQuality GetTextureQuality() const { return _currentTextureQuality; }
	// テクスチャ品質設定
	void SetTextureQuality(Terrain::TextureQuality quality) { _currentTextureQuality = quality; }

private:
    void RenderStreamOut(const RenderContext& rc);
    void RenderDynamic(const RenderContext& rc, bool writeGBuffer);
    void RenderGrass(const RenderContext& rc, bool writeGBuffer);
private:
#pragma region 描画用COMオブジェクト
    Microsoft::WRL::ComPtr<ID3D11Buffer>    _constantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> _grassConstantBuffer;

	// 通常描画用シェーダー
    Microsoft::WRL::ComPtr<ID3D11VertexShader>	_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>	_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11HullShader>	_hullShader;
    Microsoft::WRL::ComPtr<ID3D11DomainShader>	_domainShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>	_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>	_gbPixelShader;

    // デバッグ表示用ピクセルシェーダー
    Microsoft::WRL::ComPtr<ID3D11PixelShader>	_debugPixelShader;

    // 草描画用シェーダー
	VertexShader _grassVertexShader;
    Microsoft::WRL::ComPtr<ID3D11HullShader>	_grassHullShader;
    Microsoft::WRL::ComPtr<ID3D11DomainShader>	_grassDomainShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> _grassGeometryShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>	_grassPixelShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>	_grassGBPixelShader;
	// 風のゆがみテクスチャ
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _windDistortionSRV;
    // 草のテクスチャ
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _grassColorSRV;

    // ストリームアウト用
    Microsoft::WRL::ComPtr<ID3D11HullShader>	    _streamOutHullShader;
    Microsoft::WRL::ComPtr<ID3D11DomainShader>	    _streamOutDomainShader;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>	_streamOutGeometryShader;

    Microsoft::WRL::ComPtr<ID3D11Buffer>            _streamOutCollisionVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>            _streamOutCollisionCopyBuffer;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	_streamOutCollisionGeometryShader;

    // 影描画用
	VertexShader                                _shadowVertexShader;
    Microsoft::WRL::ComPtr<ID3D11HullShader>	_shadowHullShader;
    Microsoft::WRL::ComPtr<ID3D11DomainShader>	_shadowDomainShader;
	GeometryShader                              _shadowGeometryShader;
#pragma endregion
    // Terrainの描画用情報配列
	std::vector<DrawInfo> _drawInfos;
    // 頂点書き出し用情報配列
    std::vector<DrawInfo> _exportVertexDrawInfos;
	// 草の描画用情報
	std::vector<DrawInfo> _grassDrawInfos;
    // 影描画用情報配列
    std::vector<DrawInfo> _shadowDrawInfos;
    // 描画登録時の排他制御用
    std::mutex			  _drawInfoMutex;

	// 現在のテクスチャ品質設定
	Terrain::TextureQuality _currentTextureQuality = Terrain::TextureQuality::High;

	// 定数バッファのデータ
	ConstantBuffer                          _data;
	// 草の定数バッファ
	GrassConstantBuffer _dataGrass;
    // 草を描画するか
	bool _isDrawingGrass = false;
    // ワイヤーフレーム描画
	bool _isWireFrame = false;
    // GUI描画フラグ
	bool _isDrawingGui = false;
    // Mipmapを使用するか
	bool _isUsingMipmap = false;
    // デバッグ表示フラグ
    bool _isDebugDraw = false;
};