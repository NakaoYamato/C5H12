#pragma once

#include <vector>

#include "../../Library/Terrain/Terrain.h"

class TerrainRenderer
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

        float edgeFactor = 64.0f;    // エッジ分割数
        float innerFactor = 64.0f;    // 内部部分数
        float heightScaler = 1.0f;     // 高さ係数
        float lodDistanceMax = 200.0f;    // LOD(Level Of Detail)距離

        float emissive = 0.0f;     // エミッシブ
        float metalness = 0.63f;    // メタリック
        float roughness = 0.6f;     // ラフネス
        float padding1 = 0.0f;     // パディング
    };
    // 描画用情報
    struct DrawInfo
    {
		Terrain* terrain = nullptr;  // 描画するTerrain
		DirectX::XMFLOAT4X4 world = {}; // ワールド行列
		bool isExportingVertices = false; // 頂点情報をエクスポートするかどうか
    };

    static constexpr UINT ParameterMapSRVIndex = 6;
public:
	TerrainRenderer() = default;
	~TerrainRenderer() = default;

	// 初期化
	void Initialize(ID3D11Device* device);
    // 描画登録
	void Draw(Terrain* terrain, const DirectX::XMFLOAT4X4& world, bool isExportingVertices);
	// 描画処理
	void Render(const RenderContext& rc, bool writeGBuffer);
	// GUI描画
	void DrawGui();
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
#pragma endregion
    // Terrainの描画用情報配列
	std::vector<DrawInfo> _drawInfos;
	// 定数バッファのデータ
	ConstantBuffer                          _data;
    // GUI描画フラグ
	bool _isDrawingGui = false;
};