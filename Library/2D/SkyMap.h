#pragma once

#include "../../Library/Graphics/RenderContext.h"
#include "../../Library/Graphics/Shader.h"
#include "../../Library/Graphics/Texture.h"
#include "../../Library/Graphics/ConstantBuffer.h"

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>

// スカイマップ描画クラス
class SkyMap
{
public:
	// 定数定義
    static constexpr size_t LutMaxIndex = 3;

	// 定数バッファ用構造体
    struct Constants
    {
        DirectX::XMFLOAT4X4 inverseViewProjection;
    };
public:
    SkyMap(ID3D11Device* device,
        const wchar_t* filename,
        const wchar_t* diffuseIEM,
        const wchar_t* specularIDM);
    SkyMap() = delete;
    ~SkyMap() = default;

	// 描画
    void Blit(const RenderContext& rc);

	// GUI描画
    void DrawGui();
#pragma region アクセサ
    Texture& GetSRV()           { return _shaderResourceView; }
    Texture& GetDiffuseSRV()    { return _diffuseIEMSRV; }
    Texture& GetSpecularSRV()   { return _specularIEMSRV; }
    Texture& GetLutSRV()        { return _lutSRVs[_lutIndex]; }
#pragma endregion
private:
    VertexShader _vertexShader;
    PixelShader _pixelShader[2];

	// スカイマップテクスチャ
    Texture _shaderResourceView;

    ConstantBuffer _constantBuffer;

    // テクスチャがキューブマップかどうか
    bool _isTextureCube = false;

#pragma region PBR用
    Texture _diffuseIEMSRV;
    Texture _specularIEMSRV;
    Texture _lutSRVs[LutMaxIndex];

    // 使用するLUTのインデックス
    int _lutIndex = 1;
#pragma endregion
};