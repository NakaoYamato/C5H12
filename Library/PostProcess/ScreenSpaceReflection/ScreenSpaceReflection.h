#pragma once

#include "../PostProcessBase.h"
// SSR
class ScreenSpaceReflection : public PostProcessBase
{
public:
    struct Constants
    {
        DirectX::XMFLOAT4X4		view{};
        DirectX::XMFLOAT4X4		projection{};
        DirectX::XMFLOAT4X4		invViewProjection{};
        DirectX::XMFLOAT4X4		invProjection{};

        // 反射強度
        float refrectionIntensity = 0.5f;
        float maxDistance = 15.0f;
        float resolution = 0.3f;
        float thickness = 0.5f;
    };

public:
    ScreenSpaceReflection(ID3D11Device* device, uint32_t width, uint32_t height) :
        PostProcessBase(device, width, height, "./Data/Shader/ScreenSpaceReflectionPS.cso", sizeof(Constants))
    {
        // 初期値の設定
        _startData = GetCurrentData();
    };
    ~ScreenSpaceReflection()override {}

    // 更新処理
    void Update(float elapsedTime) override;

    // 描画処理
    // SRV : 0:color, 1:depth, 2:normal
    void Render(ID3D11DeviceContext* immediateContext,
        ID3D11ShaderResourceView** shaderResourceView,
        uint32_t startSlot, uint32_t numViews)override {
        PostProcessBase::Render(immediateContext, shaderResourceView, startSlot, numViews);
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