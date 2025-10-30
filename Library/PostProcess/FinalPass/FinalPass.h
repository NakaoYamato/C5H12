#pragma once

#include "../PostProcessBase.h"

// 画面の最終調整
class FinalPass : public PostProcessBase
{
public:
    struct Constants
    {
        // カラーフィルター
        float hueShift = 7.0f; // 色相調整
        float saturation = 1.5f; // 彩度調整
        float brightness = 1.25f; // 明度調整
        float colorFilterDummy = 0.0f;
        
        // トーンマッピング
        float exposure{ 2.76f };     // 補正値
        int toneType = 0;           // 使用するトーンタイプ
        DirectX::XMFLOAT2 tonemappingDummy;

        // ヴィネット
        DirectX::XMFLOAT4 vignetteColor = { 0.2f,0.2f,0.2f,1.0f };

        DirectX::XMFLOAT2 vignetteCenter = { 0.5f,0.5f };
        float vignetteIntensity = 0.1f;
        float vignetteSmoothness = 1.0f;

        int vignetteRounded = 1;// 1 or 0
        float vignetteRoundness = 1.0f;
        float vignetteDummy[2]{};
    };
public:
    FinalPass(ID3D11Device* device, uint32_t width, uint32_t height) :
        PostProcessBase(device, width, height, "./Data/Shader/HLSL/PostProcess/FinalPass/FinalPassPS.cso", sizeof(Constants))
    {
        // 初期値の設定
        _startData = GetCurrentData();
    };
    ~FinalPass()override {}

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