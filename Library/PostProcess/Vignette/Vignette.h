#pragma once

#include "../PostProcessBase.h"

// ヴィネット（画面の縁の色）
class Vignette : public PostProcessBase
{
public:
    struct Constants
    {
        DirectX::XMFLOAT4 vignetteColor = { 0.2f,0.2f,0.2f,1.0f };

        DirectX::XMFLOAT2 vignetteCenter = { 0.5f,0.5f };
        float vignetteIntensity = 0.1f;
        float vignetteSmoothness = 1.0f;

        int vignetteRounded = 1;// 1 or 0
        float vignetteRoundness = 1.0f;
        float vignetteDummy[2]{};
    };
public:
    Vignette(ID3D11Device* device, uint32_t width, uint32_t height) :
        PostProcessBase(device,
            width, height,
            "./Data/Shader/VignettePS.cso",
            sizeof(Constants)) {
        // 初期値の設定
        startData = GetCurrentData();
    }
    ~Vignette()override {}

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