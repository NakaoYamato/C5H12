#pragma once

#include "../PostProcessBase.h"
// 高輝度抽出
class GlowExtraction : public PostProcessBase
{
public:
    struct Constants
    {
        float extractionThreshold = 0.47f;
        float dummy[3];
    };
public:
    GlowExtraction(ID3D11Device* device, uint32_t width, uint32_t height) :
        PostProcessBase(device, width, height, "./Data/Shader/HLSL/PostProcess/GlowExtraction/GlowExtractionPS.cso", sizeof(Constants)) {
        // 初期値の設定
        _startData = GetCurrentData();
    }
    ~GlowExtraction()override {}

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
};