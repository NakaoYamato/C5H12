#pragma once

#include "../PostProcessBase.h"
// 被写体深度
class DepthOfField : public PostProcessBase
{
public:
    struct Constants
    {
        Vector4 cameraClipDistance;

        //	フォーカス距離
        float focusDistance = 13.0f;
        //	粗さ調整
        float dofRange = 56.0f;
        float dummy[2];
    };
public:
    DepthOfField(ID3D11Device* device, uint32_t width, uint32_t height) :
        PostProcessBase(device, width, height, "./Data/Shader/HLSL/PostProcess/DepthOfField/DepthOfFieldPS.cso", sizeof(Constants)) {
        // 初期値の設定
        _startData = GetCurrentData();
    }
    ~DepthOfField()override {}

    // 更新処理
    void Update(float elapsedTime) override;

    // 描画処理
    // shaderResourceView-> 0:color, 1:depth, 2:ぼかし画像
    void Render(ID3D11DeviceContext* immediateContext,
        ID3D11ShaderResourceView** shaderResourceView,
        uint32_t startSlot, uint32_t numViews) override {
        PostProcessBase::Render(immediateContext,
            shaderResourceView,
            startSlot, numViews);
    }

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