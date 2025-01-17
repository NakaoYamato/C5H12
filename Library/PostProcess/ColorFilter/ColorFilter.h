#pragma once

#include "../PostProcessBase.h"

// 色調補正
class ColorFilter : public PostProcessBase
{
public:
	struct Constants
	{
		float hueShift = 5.0f; // 色相調整
		float saturation = 1.15f; // 彩度調整
		float brightness = 1.0f; // 明度調整
		float dummy = 0.0f;
	};
public:
	ColorFilter(ID3D11Device* device, uint32_t width, uint32_t height) :
		PostProcessBase(device, width, height, "./Data/Shader/ColorFilterPS.cso", sizeof(Constants)) {};
	~ColorFilter()override {}

	// デバッグGui描画
	void DrawGui()override;

	std::unordered_map<std::string, float> GetData();
	// データのセット
	void SetData(std::unordered_map<std::string, float>& parameter) override;

private:
	// 定数バッファの更新
	void UpdateConstantBuffer(ID3D11DeviceContext* immediateContext, 
		ID3D11Buffer* constantBuffer) override;

private:
	Constants data{};
};