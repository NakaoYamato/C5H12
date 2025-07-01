#pragma once

#include <d3d11.h>
#include <memory>
#include <wrl.h>

#include "../../Library/Math/Vector.h"
#include "../Graphics/RenderContext.h"
#include "../PostProcess/FrameBuffer.h"

class Terrain
{
public:
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT2 texcoord;
    };

    struct ConstantBuffer
    {
        DirectX::XMFLOAT4X4 world = {};

		DirectX::XMFLOAT4 baseColor{ 1.0f, 1.0f, 1.0f, 1.0f }; // ベースカラー

        float edgeFactor{ 64 };// エッジ分割数
        float innerFactor{ 64 };// 内部部分数
        float heightScaler{ +1.0f };// 高さ係数
        float tillingScale{ +1.0f }; // タイリング係数

        float emissive{ 0.0f }; // エミッシブ
        float metalness{ 0.63f }; // メタリック
        float roughness{ 0.6f }; // ラフネス
        float padding[1] = { 0.0f }; // パディング
    };

    // ストリームアウト用
    struct StreamOutVertex
    {
        DirectX::XMFLOAT4 position;
        DirectX::XMFLOAT3 worldPosition;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT2 texcoord;
        DirectX::XMFLOAT4 blendRate;
    };

    static constexpr LONG StreamOutMaxVertex = 3 * 3 * 64 * 64;
    static constexpr LONG ParameterMapSize = 1024;
    static constexpr UINT ParameterMapIndex = 6;
public:
    Terrain(ID3D11Device* device);
	~Terrain() {}
    void Render(const RenderContext& rc, DirectX::XMFLOAT4X4 world, bool writeGBuffer);
    void DrawGui();

    // パラメータマップの書き出し
	void SaveParameterMap(ID3D11Device* device, ID3D11DeviceContext* dc, const wchar_t* heightMapPath);
#pragma region アクセサ
    // パラメータマップのフレームバッファを取得
    FrameBuffer* GetParameterMapFB() { return _parameterMapFB.get(); }
    // ストリームアウトデータを取得
    const std::vector<StreamOutVertex>& GetStreamOutData() const { return _streamOutData; }

    void SetStreamOut(bool streamOut) { _streamOut = streamOut; }
#pragma endregion

private:
    // 地形メッシュの頂点とインデックスを生成
    void CreateTerrainMesh(ID3D11Device* device);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer>	_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>    _indexBuffer;
    std::vector<Vertex>                     _vertices;
    std::vector<uint32_t>                   _indices;
    Microsoft::WRL::ComPtr<ID3D11Buffer>    _constantBuffer;
    ConstantBuffer                          _data;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>	_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11HullShader>	_hullShader;
	Microsoft::WRL::ComPtr<ID3D11DomainShader>	_domainShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	_gbPixelShader;
	// 地形用テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _colorSRVs[3];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _normalSRVs[3];
    // パラメータマップ（R：テクスチャ０ブレンド率、G：テクスチャ１ブレンド率、B：テクスチャ２ブレンド率、A：高さ）
    std::unique_ptr<FrameBuffer> _parameterMapFB;

    // ストリームアウト用
    bool _streamOut = false;
    Microsoft::WRL::ComPtr<ID3D11Buffer> _streamOutVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> _streamOutCopyBuffer;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>	_streamOutGeometryShader;
    std::vector<StreamOutVertex> _streamOutData;
};