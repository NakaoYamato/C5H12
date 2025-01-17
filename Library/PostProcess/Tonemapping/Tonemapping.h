#pragma once

#include "../PostProcessBase.h"

// トーンマッピング
class Tonemapping : public PostProcessBase
{
public:
    struct Constants
    {
        float exposure{ 2.0f };     // 補正値
        int toneType = 0;           // 使用するトーンタイプ
        DirectX::XMFLOAT2 dummy;
    };
public:
    Tonemapping(ID3D11Device* device, uint32_t width, uint32_t height) :
        PostProcessBase(device, width, height, "./Data/Shader/TonemappingPS.cso", sizeof(Constants)) {};
    ~Tonemapping()override {}

    // デバッグGui描画
    void DrawGui()override;

private:
    // 定数バッファの更新
    void UpdateConstantBuffer(ID3D11DeviceContext* immediateContext,
        ID3D11Buffer* constantBuffer) override;

private:
    Constants data{};
};