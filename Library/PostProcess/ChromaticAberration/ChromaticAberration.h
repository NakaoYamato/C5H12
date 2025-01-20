#pragma once

#include "../PostProcessBase.h"

// 色収差
class ChromaticAberration : public PostProcessBase
{
public:
    struct Constants
    {
        float aberrationAmount = 0.005f;
        unsigned int    maxSamples = 16;
        DirectX::XMFLOAT2 chromaticAberrationDummy;

        DirectX::XMFLOAT4 shift[3]
        {
            {1.0f,0.0f,0.0f,0.0f},
            {0.0f,1.0f,0.0f,0.0f},
            {0.0f,0.0f,1.0f,0.0f}
        };
    };
public:
    ChromaticAberration(ID3D11Device* device, uint32_t width, uint32_t height) :
        PostProcessBase(device, width, height, "./Data/Shader/ChromaticAberrationPS.cso", sizeof(Constants)) {
        // 初期値の設定
        startData = GetCurrentData();
    };
    ~ChromaticAberration()override {}

    // 更新処理
    void Update(float elapsedTime)override;

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
};