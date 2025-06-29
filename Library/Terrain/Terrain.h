#pragma once

#include <d3d11.h>
#include <memory>
#include <wrl.h>

#include "../../Library/Math/Vector.h"
#include "../Graphics/RenderContext.h"

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

        float edgeFactor{ 64 };// エッジ分割数
        float innerFactor{ 64 };// 内部部分数
        float heightScaler{ +1.0f };// 高さ係数
        float tillingScale{ +1.0f }; // タイリング係数

        float emissive{ 0.0f }; // エミッシブ
        float metalness{ 0.0f }; // メタリック
        float roughness{ 0.5f }; // ラフネス
        float padding[1] = { 0.0f }; // パディング
    };

public:
    Terrain(ID3D11Device* device);
	~Terrain() {}
    void Render(const RenderContext& rc, DirectX::XMFLOAT4X4 world, bool writeGBuffer);
    void DrawGui();

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
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _baseSRVs[3];
    // ハイトマップ
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _heightMapSRV;
};