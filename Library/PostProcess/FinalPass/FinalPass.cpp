#include "FinalPass.h"

#include <imgui.h>

void FinalPass::Update(float elapsedTime)
{
}

void FinalPass::DrawGui()
{
	if (ImGui::Begin(u8"FinalPass"))
	{
		if (ImGui::Button("reset"))
			ClearData();
		ImGui::Separator();

		if (ImGui::TreeNode(u8"色調補正"))
		{
			ImGui::DragFloat("hueShift", &_data.hueShift, 1.0f);
			ImGui::DragFloat("saturation", &_data.saturation, 0.01f);
			ImGui::DragFloat("brightness", &_data.brightness, 0.01f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode(u8"トーンマッピング"))
		{
			ImGui::SliderFloat("exposure", &_data.exposure, 0.0f, 10.0f);
			ImGui::SliderInt("toneType", &_data.toneType, 0, 2);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode(u8"ヴィネット"))
		{
			ImGui::ColorEdit3("color", &_data.vignetteColor.x);
			ImGui::SliderFloat2("center", &_data.vignetteCenter.x, 0, 1);
			float vignetteIntensity = _data.vignetteIntensity / 3.0f;
			ImGui::SliderFloat("intensity", &vignetteIntensity, 0.0f, +1.0f);
			_data.vignetteIntensity = vignetteIntensity * 3.0f;
			float vignetteSmoothness = _data.vignetteSmoothness / 5.0f;
			ImGui::SliderFloat("smoothness", &vignetteSmoothness, 0.0f, +1.0f);
			_data.vignetteSmoothness = std::max<float>(0.000001f, vignetteSmoothness * 5.0f);
			bool roundedFlag = _data.vignetteRounded == 1;
			ImGui::Checkbox("rounded", &roundedFlag);
			_data.vignetteRounded = roundedFlag ? 1 : 0;
			float vignetteRoundness = (-_data.vignetteRoundness + 6.0f) / 5.0f;
			ImGui::SliderFloat("roundness", &vignetteRoundness, 0.0f, +1.0f);
			_data.vignetteRoundness = 6.0f * (1.0f - vignetteRoundness) + vignetteRoundness;
			ImGui::TreePop();
		}

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
std::unordered_map<std::string, float> FinalPass::GetCurrentData()
{
	std::unordered_map<std::string, float> parameter;

	// カラーフィルター
	parameter["hueShift"] = _data.hueShift;
	parameter["saturation"] = _data.saturation;
	parameter["brightness"] = _data.brightness;

	// トーンマッピング
	parameter["exposure"] = _data.exposure;
	parameter["toneType"] = (float)_data.toneType;

	// ヴィネット
	parameter["color.x"] = _data.vignetteColor.x;
	parameter["color.y"] = _data.vignetteColor.y;
	parameter["color.z"] = _data.vignetteColor.z;
	parameter["color.w"] = _data.vignetteColor.w;
	parameter["center.x"] = _data.vignetteCenter.x;
	parameter["center.y"] = _data.vignetteCenter.y;
	parameter["intensity"] = _data.vignetteIntensity / 3.0f;
	parameter["smoothness"] = _data.vignetteSmoothness / 5.0f;
	parameter["rounded"] = (float)_data.vignetteRounded;
	parameter["roundness"] = (-_data.vignetteRoundness + 6.0f) / 5.0f;

	return parameter;
}

void FinalPass::SetData(std::unordered_map<std::string, float>& parameter)
{
	// カラーフィルター
	{
		{
			auto iter = parameter.find("hueShift");
			if (iter != parameter.end())
				_data.hueShift = (*iter).second;
		}
		{
			auto iter = parameter.find("saturation");
			if (iter != parameter.end())
				_data.saturation = (*iter).second;
		}
		{
			auto iter = parameter.find("brightness");
			if (iter != parameter.end())
				_data.brightness = (*iter).second;
		}
	}

	// トーンマッピング
	{
		{
			auto iter = parameter.find("exposure");
			if (iter != parameter.end())
				_data.exposure = (*iter).second;
		}
		{
			auto iter = parameter.find("toneType");
			if (iter != parameter.end())
				_data.toneType = (int)(*iter).second;
		}
	}

	// ヴィネット
	{
		{
			auto iter = parameter.find("color.x");
			if (iter != parameter.end())
				_data.vignetteColor.x = (*iter).second;
		}
		{
			auto iter = parameter.find("color.y");
			if (iter != parameter.end())
				_data.vignetteColor.y = (*iter).second;
		}
		{
			auto iter = parameter.find("color.z");
			if (iter != parameter.end())
				_data.vignetteColor.z = (*iter).second;
		}
		{
			auto iter = parameter.find("color.w");
			if (iter != parameter.end())
				_data.vignetteColor.w = (*iter).second;
		}

		{
			auto iter = parameter.find("center.x");
			if (iter != parameter.end())
				_data.vignetteCenter.x = (*iter).second;
		}
		{
			auto iter = parameter.find("center.y");
			if (iter != parameter.end())
				_data.vignetteCenter.y = (*iter).second;
		}

		{
			auto iter = parameter.find("intensity");
			if (iter != parameter.end())
				_data.vignetteIntensity = (*iter).second * 3.0f;
		}

		{
			auto iter = parameter.find("smoothness");
			if (iter != parameter.end())
				_data.vignetteSmoothness = (*iter).second * 5.0f;
		}

		{
			auto iter = parameter.find("rounded");
			if (iter != parameter.end())
				_data.vignetteRounded = (int)(*iter).second;
		}

		{
			auto iter = parameter.find("roundness");
			if (iter != parameter.end())
				_data.vignetteRoundness = 6.0f * (1.0f - (*iter).second) + (*iter).second;
		}
	}
}

void FinalPass::UpdateConstantBuffer(ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer)
{
	immediateContext->UpdateSubresource(constantBuffer, 0, 0, &_data, 0, 0);
}
