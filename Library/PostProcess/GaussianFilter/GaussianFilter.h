#pragma once

#include "../PostProcessBase.h"

// ガウスフィルター（ぼかし）
class GaussianFilter : public PostProcessBase
{
public:
#define KERNEL_MAX 25
    struct Constants
    {
        DirectX::XMFLOAT4 weights[KERNEL_MAX * KERNEL_MAX];
        float kernelSize;
        DirectX::XMFLOAT2 texcel;
        float dummy;
    };

    struct Datas
    {
        int kernelSize = 3;
        float sigma = 10.0f;    // 標準偏差(ぼかし量)
        DirectX::XMFLOAT2 textureSize = { 1920.0f,1080.0f };
    };

public:
    GaussianFilter(ID3D11Device* device, uint32_t width, uint32_t height) :
        PostProcessBase(device, width, height, "./Data/Shader/GaussianFilterPS.cso", sizeof(Constants)) {
        // 初期値の設定
        startData = GetCurrentData();
    };
    ~GaussianFilter()override {}

    // 更新処理
    void Update(float elapsedTime)override;

    // デバッグGui描画
    void DrawGui()override;

private:
    // 定数バッファの更新
    void UpdateConstantBuffer(ID3D11DeviceContext* immediateContext,
        ID3D11Buffer* constantBuffer) override;

    // フィルターの計算
    void CalcGaussianFilterConstant(Constants& constant, const Datas data);

    // 現在のデータの取得
    std::unordered_map<std::string, float> GetCurrentData() override;
    // データのセット
    void SetData(std::unordered_map<std::string, float>& parameter) override;
private:
    Constants constant{};
    Datas data{};
};