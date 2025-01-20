#pragma once

#include "../PostProcessBase.h"

// 河瀬式ブルーム
class Bloom : public PostProcessBase
{
public:
    struct Constants
    {
        float extractionThreshold = 0.47f;
        float intensity = 0.05f;
        float something[2];
    };

    static constexpr size_t DownSampledCount = 6;
public:
    Bloom(ID3D11Device* device, uint32_t width, uint32_t height);
    ~Bloom()override {}

    // 描画処理
    void Render(ID3D11DeviceContext* immediateContext,
        ID3D11ShaderResourceView** shaderResourceView,
        uint32_t startSlot, uint32_t numViews)override;

    // デバッグGui描画
    void DrawGui()override;

    // 現在のデータの取得
    std::unordered_map<std::string, float> GetCurrentData() override;
    // データのセット
    void SetData(std::unordered_map<std::string, float>& parameter) override;
private:
    // 定数バッファの更新
    void UpdateConstantBuffer(ID3D11DeviceContext* immediateContext,
        ID3D11Buffer* constantBuffer) override;

private:
    Constants data{};

    // 高輝度抽出用
    std::unique_ptr<FrameBuffer> glowExtraction;
    // ブルーム用
    std::unique_ptr<FrameBuffer> bloomFrameBuffer[DownSampledCount][2];

    enum BLOOM_PIXEL_TYPE
    {
        GLOW_EXTRACTION_PS,
        DOWNSAMPLING_PS,
        HORIZONTAL_PS,
        VERTICAL_PS,
        //UPSAMPLING_PS, // -> PostProcessBase側で行う
        BLOOM_PIXEL_TYPE_MAX
    };
    Microsoft::WRL::ComPtr<ID3D11PixelShader> bloomPixelShaders[BLOOM_PIXEL_TYPE::BLOOM_PIXEL_TYPE_MAX];

    // ブルーム用ステート
    Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
};