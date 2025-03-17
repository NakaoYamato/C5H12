#pragma once

#include "../PostProcessBase.h"

// ガウスフィルター（ぼかし）
class GaussianFilter : public PostProcessBase
{
public:
    struct Constants
    {
        float intensity = 0.05f;
        float something[3];
    };

    static constexpr size_t DownSampledCount = 6;
public:
    GaussianFilter(ID3D11Device* device, uint32_t width, uint32_t height);
    ~GaussianFilter()override {}

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
    Constants _data{};

    std::unique_ptr<FrameBuffer> _pingPongFrameBuffer[DownSampledCount][2];

    enum PIXEL_SHADER_TYPE
    {
        DOWNSAMPLING_PS,
        HORIZONTAL_PS,
        VERTICAL_PS,
        //UPSAMPLING_PS, // -> PostProcessBase側で行う

        BLOOM_PIXEL_TYPE_MAX
    };
    Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixelShaders[PIXEL_SHADER_TYPE::BLOOM_PIXEL_TYPE_MAX];

    // 専用ステート
    Microsoft::WRL::ComPtr<ID3D11SamplerState>		_samplerState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> _depthStencilState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> _rasterizerState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> _blendState;
};