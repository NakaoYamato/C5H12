#include "Tonemapping.h"
#include <imgui.h>

void Tonemapping::DrawGui()
{
	if (ImGui::Begin(u8"トーンマッピング"))
	{
		if (ImGui::Button("reset"))
			ClearData();
		ImGui::SliderFloat("exposure", &data.exposure, 0.0f, 10.0f);
		ImGui::SliderInt("toneType", &data.toneType, 0, 2);
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
std::unordered_map<std::string, float> Tonemapping::GetCurrentData()
{
	std::unordered_map<std::string, float> parameter;
	parameter["exposure"] = data.exposure;
	parameter["toneType"] = (float)data.toneType;
	return parameter;
}

// データのセット
void Tonemapping::SetData(std::unordered_map<std::string, float>& parameter)
{
	{
		auto iter = parameter.find("exposure");
		if (iter != parameter.end())
			data.exposure = (*iter).second;
	}
	{
		auto iter = parameter.find("toneType");
		if (iter != parameter.end())
			data.toneType = (int)(*iter).second;
	}
}

void Tonemapping::UpdateConstantBuffer(ID3D11DeviceContext* immediateContext,
    ID3D11Buffer* constantBuffer)
{
    immediateContext->UpdateSubresource(constantBuffer, 0, 0, &data, 0, 0);
}
