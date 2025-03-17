#include "GlowExtraction.h"

#include <imgui.h>

void GlowExtraction::DrawGui()
{
	if (ImGui::Begin(u8"çÇãPìxíäèo"))
	{
		if (ImGui::Button("reset"))
			ClearData();
		ImGui::SliderFloat("extractionThreshold", &_data.extractionThreshold, +0.0f, +1.0f);
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

std::unordered_map<std::string, float> GlowExtraction::GetCurrentData()
{
	std::unordered_map<std::string, float> parameter;
	parameter["extractionThreshold"] = _data.extractionThreshold;
	return parameter;
}

void GlowExtraction::SetData(std::unordered_map<std::string, float>& parameter)
{
	{
		auto iter = parameter.find("extractionThreshold");
		if (iter != parameter.end())
			_data.extractionThreshold = (*iter).second;
	}
}

void GlowExtraction::UpdateConstantBuffer(ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer)
{
	immediateContext->UpdateSubresource(constantBuffer, 0, 0, &_data, 0, 0);
}
