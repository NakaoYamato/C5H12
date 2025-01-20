#pragma once

#include "../PostProcessBase.h"

// ラジアルブラー
class RadialBlur : public PostProcessBase
{
public:
    struct Constants
    {
        float blurRadius = 0.0f;
        int blurSamplingCount = 10;
        DirectX::XMFLOAT2 blurCenter = { 0.5f,0.5f };// UV Space

        float blurMaskRadius = 300.0f;
        DirectX::XMFLOAT3 blurDummy{};
    };
public:
    RadialBlur(ID3D11Device* device, uint32_t width, uint32_t height) :
        PostProcessBase(device, width, height, "./Data/Shader/RadialBlurPS.cso", sizeof(Constants)) {
        // 初期値の設定
        startData = GetCurrentData();
    };
    ~RadialBlur()override {}

    // 更新処理
    void Update(float elapsedTime)override;

    // デバッグGui描画
    void DrawGui()override;

    // 現在のデータの取得
    std::unordered_map<std::string, float> GetCurrentData();
    // データのセット
    void SetData(std::unordered_map<std::string, float>& parameter) override;
private:
    // 定数バッファの更新
    void UpdateConstantBuffer(ID3D11DeviceContext* immediateContext,
        ID3D11Buffer* constantBuffer) override;

private:
    Constants data{};
};