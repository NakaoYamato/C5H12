#include "Vignette.h"
#include <imgui.h>

// 更新処理
void Vignette::Update(float elapsedTime)
{
}

// デバッグGui描画
void Vignette::DrawGui()
{
	if (ImGui::Begin(u8"ヴィネット"))
	{
		ImGui::ColorEdit3("color", &data.vignetteColor.x);
		ImGui::SliderFloat2("center", &data.vignetteCenter.x, 0, 1);
		float vignetteIntensity = data.vignetteIntensity / 3.0f;
		ImGui::SliderFloat("intensity", &vignetteIntensity, 0.0f, +1.0f);
		data.vignetteIntensity = vignetteIntensity * 3.0f;
		float vignetteSmoothness = data.vignetteSmoothness / 5.0f;
		ImGui::SliderFloat("smoothness", &vignetteSmoothness, 0.0f, +1.0f);
		data.vignetteSmoothness = std::max<float>(0.000001f, vignetteSmoothness * 5.0f);
		bool roundedFlag = data.vignetteRounded == 1;
		ImGui::Checkbox("rounded", &roundedFlag);
		data.vignetteRounded = roundedFlag ? 1 : 0;
		float vignetteRoundness = (-data.vignetteRoundness + 6.0f) / 5.0f;
		ImGui::SliderFloat("roundness", &vignetteRoundness, 0.0f, +1.0f);
		data.vignetteRoundness = 6.0f * (1.0f - vignetteRoundness) + vignetteRoundness;
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

std::unordered_map<std::string, float> Vignette::GetData()
{
	std::unordered_map<std::string, float> parameter;
	parameter["color.x"] = data.vignetteColor.x;
	parameter["color.y"] = data.vignetteColor.y;
	parameter["color.z"] = data.vignetteColor.z;
	parameter["color.w"] = data.vignetteColor.w;
	parameter["center.x"] = data.vignetteCenter.x;
	parameter["center.y"] = data.vignetteCenter.y;
	parameter["intensity"] = data.vignetteIntensity / 3.0f;
	parameter["smoothness"] = data.vignetteSmoothness / 5.0f;
	parameter["rounded"] = (float)data.vignetteRounded;
	parameter["roundness"] = (-data.vignetteRoundness + 6.0f) / 5.0f;
	return parameter;
}

// データのセット
void Vignette::SetData(std::unordered_map<std::string, float>& parameter)
{
	{
		auto iter = parameter.find("color.x");
		if (iter != parameter.end())
			data.vignetteColor.x = (*iter).second;
	}
	{
		auto iter = parameter.find("color.y");
		if (iter != parameter.end())
			data.vignetteColor.y = (*iter).second;
	}
	{
		auto iter = parameter.find("color.z");
		if (iter != parameter.end())
			data.vignetteColor.z = (*iter).second;
	}
	{
		auto iter = parameter.find("color.w");
		if (iter != parameter.end())
			data.vignetteColor.w = (*iter).second;
	}

	{
		auto iter = parameter.find("center.x");
		if (iter != parameter.end())
			data.vignetteCenter.x = (*iter).second;
	}
	{
		auto iter = parameter.find("center.y");
		if (iter != parameter.end())
			data.vignetteCenter.y = (*iter).second;
	}

	{
		auto iter = parameter.find("intensity");
		if (iter != parameter.end())
			data.vignetteIntensity = (*iter).second * 3.0f;
	}

	{
		auto iter = parameter.find("smoothness");
		if (iter != parameter.end())
			data.vignetteSmoothness = (*iter).second * 5.0f;
	}

	{
		auto iter = parameter.find("rounded");
		if (iter != parameter.end())
			data.vignetteRounded = (int)(*iter).second;
	}

	{
		auto iter = parameter.find("roundness");
		if (iter != parameter.end())
			data.vignetteRoundness = 6.0f * (1.0f - (*iter).second) + (*iter).second;
	}
}

// 定数バッファの更新
void Vignette::UpdateConstantBuffer(ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer)
{
	immediateContext->UpdateSubresource(constantBuffer, 0, 0, &data, 0, 0);
}
