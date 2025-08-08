#pragma once

#include "../PostProcessBase.h"

// FXAA3_11
class FXAA : public PostProcessBase
{
public:
    struct Constants
    {
        // 画面の大きさの逆数
        DirectX::XMFLOAT2 invScreenSize;

        // エイリアス除去量
        //   1.00 - upper limit (softer)
        //   0.75 - default amount of filtering
        //   0.50 - lower limit (sharper, less sub-pixel aliasing removal)
        //   0.25 - almost off
        //   0.00 - completely off
        float subpix = 0.75f;

        // エッジしきい値
        //   0.333 - too little (faster)
        //   0.250 - low quality
        //   0.166 - default
        //   0.125 - high quality 
        //   0.063 - overkill (slower)
        float edgeThreshold = 0.333f;

        // エッジしきい値の下限
        //   0.0833 - upper limit (default, the start of visible unfiltered edges)
        //   0.0625 - high quality (faster)
        //   0.0312 - visible limit (slower)
        float edgeThresholdMin = 0.0833f;

        // 使用フラグ
        float useFlag = 1.0f;

        DirectX::XMFLOAT2 dummy;
    };
public:
    FXAA(ID3D11Device* device, uint32_t width, uint32_t height) :
        PostProcessBase(device, width, height, "./Data/Shader/HLSL/PostProcess/FXAA/FXAAPS.cso", sizeof(Constants))
    {
        _data.invScreenSize.x = 1.0f / static_cast<float>(width);
        _data.invScreenSize.y = 1.0f / static_cast<float>(height);
        // 初期値の設定
        _startData = GetCurrentData();
    };
    ~FXAA()override {}

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