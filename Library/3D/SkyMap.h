#pragma once

#include "../Graphics/RenderContext.h"

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>

class SkyMap
{
public:
    SkyMap(ID3D11Device* device, const wchar_t* filename);
    SkyMap() = delete;
    ~SkyMap() = default;

    void Blit(const RenderContext& rc);

    ID3D11ShaderResourceView* const* GetSRV() {
        return shaderResourceView.GetAddressOf();
    }
private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader[2];

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;

    struct Constants
    {
        DirectX::XMFLOAT4X4 inverseViewProjection;
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

    bool isTextureCube = false;
};