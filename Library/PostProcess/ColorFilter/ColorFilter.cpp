#include "ColorFilter.h"
#include <imgui.h>

// デバッグGui描画
void ColorFilter::DrawGui()
{
	if (ImGui::Begin(u8"色調補正"))
	{
		if (ImGui::Button("reset"))
			ClearData();
		ImGui::DragFloat("hueShift", &data.hueShift, 1.0f);
		ImGui::DragFloat("saturation", &data.saturation, 0.01f);
		ImGui::DragFloat("brightness", &data.brightness, 0.01f);
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

std::unordered_map<std::string, float> ColorFilter::GetCurrentData()
{
	std::unordered_map<std::string, float> parameter;
	parameter["hueShift"] = data.hueShift;
	parameter["saturation"] = data.saturation;
	parameter["brightness"] = data.brightness;
	return parameter;
}

void ColorFilter::SetData(std::unordered_map<std::string, float>& parameter)
{
	{
		auto iter = parameter.find("hueShift");
		if (iter != parameter.end())
			data.hueShift = (*iter).second;
	}
	{
		auto iter = parameter.find("saturation");
		if (iter != parameter.end())
			data.saturation = (*iter).second;
	}
	{
		auto iter = parameter.find("brightness");
		if (iter != parameter.end())
			data.brightness = (*iter).second;
	}
}

// 定数バッファの更新
void ColorFilter::UpdateConstantBuffer(ID3D11DeviceContext* immediateContext,
	ID3D11Buffer* constantBuffer)
{
	immediateContext->UpdateSubresource(constantBuffer, 0, 0, &data, 0, 0);
}
