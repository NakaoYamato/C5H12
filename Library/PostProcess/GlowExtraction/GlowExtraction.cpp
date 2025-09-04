#include "GlowExtraction.h"

#include <imgui.h>

void GlowExtraction::DrawGui()
{
	ImGui::SliderFloat("extractionThreshold", &_data.extractionThreshold, +0.0f, +1.0f);
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
