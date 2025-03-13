#pragma once

#include "../Graphics/RenderContext.h"

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>

class SkyMap
{
public:
    SkyMap(ID3D11Device* device, const wchar_t* filename, const wchar_t* diffuseIEM, const wchar_t* specularIDM);
    SkyMap() = delete;
    ~SkyMap() = default;

    void Blit(const RenderContext& rc);

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetSRV() { return shaderResourceView_; }
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetDiffuseSRV() { return diffuseIEMSRV_; }
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetSpecularSRV() { return specularIEMSRV_; }
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetLutGGXSRV() { return lutGGXSRV_; }

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_[2];

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView_;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseIEMSRV_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specularIEMSRV_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> lutGGXSRV_;

    struct Constants
    {
        DirectX::XMFLOAT4X4 inverseViewProjection;
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer_;

    bool isTextureCube_
 = false;
};