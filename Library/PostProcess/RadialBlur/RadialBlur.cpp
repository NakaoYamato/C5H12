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
		ImGui::SliderFloat("blurRadius", &_data.blurRadius, 0.001f, +100.0f);
		ImGui::SliderInt("blurSamplingCount", &_data.blurSamplingCount, 1, 100);
		ImGui::SliderFloat2("blurCenter", &_data.blurCenter.x, 0.0f, 1.0f);
		ImGui::SliderFloat("blurMaskRadius", &_data.blurMaskRadius, 0, 300.0f);
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
	parameter["blurRadius"] = _data.blurRadius;
	parameter["blurSamplingCount"] = (float)_data.blurSamplingCount;
	parameter["blurCenter.x"] = _data.blurCenter.x;
	parameter["blurCenter.y"] = _data.blurCenter.y;
	parameter["blurMaskRadius"] = _data.blurMaskRadius;
	return parameter;
}

// データのセット
void RadialBlur::SetData(std::unordered_map<std::string, float>& parameter)
{
	{
		auto iter = parameter.find("blurRadius");
		if (iter != parameter.end())
			_data.blurRadius = (*iter).second;
	}
	{
		auto iter = parameter.find("blurSamplingCount");
		if (iter != parameter.end())
			_data.blurSamplingCount = (int)(*iter).second;
	}
	{
		auto iter = parameter.find("blurCenter.x");
		if (iter != parameter.end())
			_data.blurCenter.x = (*iter).second;
	}
	{
		auto iter = parameter.find("blurCenter.y");
		if (iter != parameter.end())
			_data.blurCenter.y = (*iter).second;
	}
	{
		auto iter = parameter.find("blurMaskRadius");
		if (iter != parameter.end())
			_data.blurMaskRadius = (*iter).second;
	}
}

// 定数バッファの更新
void RadialBlur::UpdateConstantBuffer(ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer)
{
	immediateContext->UpdateSubresource(constantBuffer, 0, 0, &_data, 0, 0);
}
