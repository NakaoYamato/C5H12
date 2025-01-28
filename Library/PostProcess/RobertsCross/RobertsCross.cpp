#include "RobertsCross.h"

#include <imgui.h>

void RobertsCross::DrawGui()
{
	if (ImGui::Begin(u8"RobertsCross"))
	{
		ImGui::Checkbox("enable", reinterpret_cast<bool*>(&data.enable));
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

std::unordered_map<std::string, float> RobertsCross::GetCurrentData()
{
	std::unordered_map<std::string, float> parameter;
	parameter["enable"] = (float)data.enable;
	parameter["lineWidth"] = (float)data.lineWidth;
	return parameter;
}

void RobertsCross::SetData(std::unordered_map<std::string, float>& parameter)
{
	{
		auto iter = parameter.find("enable");
		if (iter != parameter.end())
			data.enable = (int)(*iter).second;
	}
	{
		auto iter = parameter.find("lineWidth");
		if (iter != parameter.end())
			data.lineWidth = (UINT32)(*iter).second;
	}
}

void RobertsCross::UpdateConstantBuffer(ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer)
{
	immediateContext->UpdateSubresource(constantBuffer, 0, 0, &data, 0, 0);
}
