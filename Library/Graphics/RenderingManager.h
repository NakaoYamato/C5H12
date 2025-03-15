#pragma once
#include <d3d11.h>
#include <memory>

#include "../2D/Sprite.h"
#include "../PostProcess/FrameBuffer.h"
#include "../PostProcess/CascadedShadowMap/CascadedShadowMap.h"
#include "GBuffer.h"

enum class FullscreenQuadPS
{
    EmbeddedPS,	// 通常の処理
    CascadedPS,	// カスケードシャドウ
    DeferredRenderingPS,

    FullscreenQuadPSMax,
};

class RenderingManager
{
public:
    RenderingManager(ID3D11Device* device, UINT width, UINT height);
    ~RenderingManager() {}

    // フレームバッファ取得
    [[nodiscard]] FrameBuffer* GetFrameBuffer(int index) { return _frameBufferes[index].get(); }

    // カスケードシャドウマップ取得
    [[nodiscard]] CascadedShadowMap* GetCascadedShadowMap() { return _cascadedShadowMap.get(); }

    // マルチレンダーターゲット取得
    [[nodiscard]] GBuffer* GetGBuffer() { return _gBuffer.get(); }

    // PIXEL_SHADER_TYPEでピクセルシェーダを指定して描画
    void Blit(ID3D11DeviceContext* immediateContext,
        ID3D11ShaderResourceView** shaderResourceView,
        uint32_t startSlot, uint32_t numViews,
        FullscreenQuadPS shaderType = FullscreenQuadPS::EmbeddedPS);

    // この関数の使用者側でピクセルシェーダを指定して描画
    void Blit(ID3D11DeviceContext* immediateContext,
        ID3D11ShaderResourceView** shaderResourceView,
        uint32_t startSlot, uint32_t numViews,
        ID3D11PixelShader* pixelShader);

    // ImGui描画
    void DrawGui();

    // デファードレンダリングかどうか
    bool RenderingDeferred()const { return _renderingDeferred; }
private:
    // オフスクリーンレンダリングの管理者
    std::unique_ptr<FrameBuffer> _frameBufferes[4];
    std::unique_ptr<Sprite> _fullscreenQuad;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixelShaders[static_cast<int>(FullscreenQuadPS::FullscreenQuadPSMax)];
    // カスケードシャドウマップ
    std::unique_ptr<CascadedShadowMap> _cascadedShadowMap;
    // マルチレンダーターゲットの管理者
    std::unique_ptr<GBuffer> _gBuffer;

    // デバッグ用
    bool _renderingDeferred = true;
    bool _drawCSMGui = false;
    bool _drawGBGui = false;
};