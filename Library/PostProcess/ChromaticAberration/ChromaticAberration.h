#pragma once

#include "../PostProcessBase.h"

// 色収差
class ChromaticAberration : public PostProcessBase
{
public:
    struct Constants
    {
        float chromaticAberrationAmount = 0.005f;
        unsigned int    chromaticAberrationMaxSamples = 16;
        DirectX::XMFLOAT2 chromaticAberrationDummy;

        DirectX::XMFLOAT4 chromaticAberrationShift[3]
        {
            {1.0f,0.0f,0.0f,0.0f},
            {0.0f,1.0f,0.0f,0.0f},
            {0.0f,0.0f,1.0f,0.0f}
        };
    };
public:
    ChromaticAberration(ID3D11Device* device, uint32_t width, uint32_t height) :
        PostProcessBase(device, width, height, "./Data/Shader/ChromaticAberrationPS.cso", sizeof(Constants)) {};
    ~ChromaticAberration()override {}

    // 更新処理
    void Update(float elapsedTime)override;

    // デバッグGui描画
    void DrawGui()override;

private:
    // 定数バッファの更新
    void UpdateConstantBuffer(ID3D11DeviceContext* immediateContext,
        ID3D11Buffer* constantBuffer) override;

private:
    Constants data{};
};