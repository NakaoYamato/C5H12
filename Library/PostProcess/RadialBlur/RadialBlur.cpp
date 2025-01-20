#include "RadialBlur.h"
#include <imgui.h>

// 更新処理
void RadialBlur::Update(float elapsedTime)
{
}

// デバッグGui描画
void RadialBlur::DrawGui()
{
	if (ImGui::Begin(u8"ラジアルブラー"))
	{
		if (ImGui::Button("reset"))
			ClearData();
		ImGui::SliderFloat("blurRadius", &data.blurRadius, 0.001f, +100.0f);
		ImGui::SliderInt("blurSamplingCount", &data.blurSamplingCount, 1, 100);
		ImGui::SliderFloat2("blurCenter", &data.blurCenter.x, 0.0f, 1.0f);
		ImGui::SliderFloat("blurMaskRadius", &data.blurMaskRadius, 0, 300.0f);
		if (ImGui::TreeNode("Resource"))
		{
			static float textureSize = 512.0f;
			ImGui::DragFloat("TextureSize", &textureSize);
			ImGui::Image(GetColorSRV().Get(),
				{ textureSize ,textureSize });

			ImGui::TreePop();
		}
	}
	ImGui::End();
}

// 現在のデータの取得
std::unordered_map<std::string, float> RadialBlur::GetCurrentData()
{
	std::unordered_map<std::string, float> parameter;
	parameter["blurRadius"] = data.blurRadius;
	parameter["blurSamplingCount"] = (float)data.blurSamplingCount;
	parameter["blurCenter.x"] = data.blurCenter.x;
	parameter["blurCenter.y"] = data.blurCenter.y;
	parameter["blurMaskRadius"] = data.blurMaskRadius;
	return parameter;
}

// データのセット
void RadialBlur::SetData(std::unordered_map<std::string, float>& parameter)
{
	{
		auto iter = parameter.find("blurRadius");
		if (iter != parameter.end())
			data.blurRadius = (*iter).second;
	}
	{
		auto iter = parameter.find("blurSamplingCount");
		if (iter != parameter.end())
			data.blurSamplingCount = (int)(*iter).second;
	}
	{
		auto iter = parameter.find("blurCenter.x");
		if (iter != parameter.end())
			data.blurCenter.x = (*iter).second;
	}
	{
		auto iter = parameter.find("blurCenter.y");
		if (iter != parameter.end())
			data.blurCenter.y = (*iter).second;
	}
	{
		auto iter = parameter.find("blurMaskRadius");
		if (iter != parameter.end())
			data.blurMaskRadius = (*iter).second;
	}
}

// 定数バッファの更新
void RadialBlur::UpdateConstantBuffer(ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer)
{
	immediateContext->UpdateSubresource(constantBuffer, 0, 0, &data, 0, 0);
}
