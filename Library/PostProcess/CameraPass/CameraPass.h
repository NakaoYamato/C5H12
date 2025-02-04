#pragma once

#include "../PostProcessBase.h"

// カメラによる画面の影響
class CameraPass : public PostProcessBase
{
public:
    struct Constants
    {
        float aberrationAmount = 0.000f;
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
    CameraPass(ID3D11Device* device, uint32_t width, uint32_t height) :
        PostProcessBase(device, width, height, "./Data/Shader/CameraPassPS.cso", sizeof(Constants)) {
        // 初期値の設定
        startData = GetCurrentData();
    };
    ~CameraPass()override {}

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