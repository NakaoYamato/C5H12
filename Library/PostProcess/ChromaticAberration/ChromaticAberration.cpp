#include "ChromaticAberration.h"
#include <imgui.h>

// 更新処理
void ChromaticAberration::Update(float elapsedTime)
{
}

// デバッグGui描画
void ChromaticAberration::DrawGui()
{
	if (ImGui::Begin(u8"色収差"))
	{
		if (ImGui::Button("reset"))
			ClearData();
		float chromaticAberrationAmount = data.aberrationAmount * 100.0f;
		ImGui::SliderFloat("amount", &chromaticAberrationAmount, 0.0f, +1.0f);
		data.aberrationAmount = chromaticAberrationAmount * 0.01f;
		int chromaticAberrationMaxSamples = data.maxSamples;
		ImGui::SliderInt("max sample", &chromaticAberrationMaxSamples, 0, 100);
		data.maxSamples = chromaticAberrationMaxSamples;
		ImGui::SliderFloat3("shift 0", &data.shift[0].x, 0.0f, +1.0f);
		ImGui::SliderFloat3("shift 1", &data.shift[1].x, 0.0f, +1.0f);
		ImGui::SliderFloat3("shift 2", &data.shift[2].x, 0.0f, +1.0f);
	}
	ImGui::End();
}

std::unordered_map<std::string, float> ChromaticAberration::GetCurrentData()
{
	std::unordered_map<std::string, float> parameter;
	parameter["aberrationAmount"] = data.aberrationAmount * 100.0f;
	parameter["maxSamples"] = (float)data.maxSamples;
	parameter["shift[0].x"] = data.shift[0].x;
	parameter["shift[0].y"] = data.shift[0].y;
	parameter["shift[0].z"] = data.shift[0].z;
	parameter["shift[1].x"] = data.shift[1].x;
	parameter["shift[1].y"] = data.shift[1].y;
	parameter["shift[1].z"] = data.shift[1].z;
	parameter["shift[2].x"] = data.shift[2].x;
	parameter["shift[2].y"] = data.shift[2].y;
	parameter["shift[2].z"] = data.shift[2].z;
	return parameter;
}

void ChromaticAberration::SetData(std::unordered_map<std::string, float>& parameter)
{
	{
		auto iter = parameter.find("aberrationAmount");
		if (iter != parameter.end())
			data.aberrationAmount = (*iter).second / 100.0f;
	}
	{
		auto iter = parameter.find("maxSamples");
		if (iter != parameter.end())
			data.maxSamples = (unsigned int)(*iter).second;
	}
	{
		auto iter = parameter.find("shift[0].x");
		if (iter != parameter.end())
			data.shift[0].x = (*iter).second;
	}
	{
		auto iter = parameter.find("shift[0].y");
		if (iter != parameter.end())
			data.shift[0].y = (*iter).second;
	}
	{
		auto iter = parameter.find("shift[0].z");
		if (iter != parameter.end())
			data.shift[0].z = (*iter).second;
	}
	{
		auto iter = parameter.find("shift[1].x");
		if (iter != parameter.end())
			data.shift[1].x = (*iter).second;
	}
	{
		auto iter = parameter.find("shift[1].y");
		if (iter != parameter.end())
			data.shift[1].y = (*iter).second;
	}
	{
		auto iter = parameter.find("shift[1].z");
		if (iter != parameter.end())
			data.shift[1].z = (*iter).second;
	}
	{
		auto iter = parameter.find("shift[2].x");
		if (iter != parameter.end())
			data.shift[2].x = (*iter).second;
	}
	{
		auto iter = parameter.find("shift[2].y");
		if (iter != parameter.end())
			data.shift[2].y = (*iter).second;
	}
	{
		auto iter = parameter.find("shift[2].z");
		if (iter != parameter.end())
			data.shift[2].z = (*iter).second;
	}
}

// 定数バッファの更新
void ChromaticAberration::UpdateConstantBuffer(ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer)
{
	immediateContext->UpdateSubresource(constantBuffer, 0, 0, &data, 0, 0);
}
