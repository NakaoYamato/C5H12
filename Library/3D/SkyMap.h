#pragma once

#include "../Graphics/RenderContext.h"

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>

/// <summary>
/// スカイマップ描画クラス
/// </summary>
class SkyMap
{
public:
    static constexpr size_t LUT_INDEX_MAX = 3;

public:
    SkyMap(ID3D11Device* device, const wchar_t* filename, const wchar_t* diffuseIEM, const wchar_t* specularIDM);
    SkyMap() = delete;
    ~SkyMap() = default;

    void Blit(const RenderContext& rc);

    void DrawGui();
#pragma region アクセサ
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetSRV() { return _shaderResourceView; }
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetDiffuseSRV() { return _diffuseIEMSRV; }
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetSpecularSRV() { return _specularIEMSRV; }
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetLutSRV() { return _lutSRVs[_lutIndex]; }
#pragma endregion
private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> _vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixelShader[2];

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _shaderResourceView;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _diffuseIEMSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _specularIEMSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _lutSRVs[LUT_INDEX_MAX];

    struct Constants
    {
        DirectX::XMFLOAT4X4 inverseViewProjection;
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> _constantBuffer;

    bool _isTextureCube = false;
    int _lutIndex = 1;
    bool _drawGui = false;
};