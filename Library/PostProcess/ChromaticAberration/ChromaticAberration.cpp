#include "ChromaticAberration.h"
#include <imgui.h>

// 更新処理
void ChromaticAberration::Update(float elapsedTime)
{
}

// デバッグGui描画
void ChromaticAberration::DrawGui()
{
	float chromaticAberrationAmount = _data.aberrationAmount * 100.0f;
	ImGui::SliderFloat("amount", &chromaticAberrationAmount, 0.0f, +1.0f);
	_data.aberrationAmount = chromaticAberrationAmount * 0.01f;
	int chromaticAberrationMaxSamples = _data.maxSamples;
	ImGui::SliderInt("max sample", &chromaticAberrationMaxSamples, 0, 100);
	_data.maxSamples = chromaticAberrationMaxSamples;
	ImGui::SliderFloat3("shift 0", &_data.shift[0].x, 0.0f, +1.0f);
	ImGui::SliderFloat3("shift 1", &_data.shift[1].x, 0.0f, +1.0f);
	ImGui::SliderFloat3("shift 2", &_data.shift[2].x, 0.0f, +1.0f);
}

std::unordered_map<std::string, float> ChromaticAberration::GetCurrentData()
{
	std::unordered_map<std::string, float> parameter;
	parameter["aberrationAmount"] = _data.aberrationAmount * 100.0f;
	parameter["maxSamples"] = (float)_data.maxSamples;
	parameter["shift[0].x"] = _data.shift[0].x;
	parameter["shift[0].y"] = _data.shift[0].y;
	parameter["shift[0].z"] = _data.shift[0].z;
	parameter["shift[1].x"] = _data.shift[1].x;
	parameter["shift[1].y"] = _data.shift[1].y;
	parameter["shift[1].z"] = _data.shift[1].z;
	parameter["shift[2].x"] = _data.shift[2].x;
	parameter["shift[2].y"] = _data.shift[2].y;
	parameter["shift[2].z"] = _data.shift[2].z;
	return parameter;
}

void ChromaticAberration::SetData(std::unordered_map<std::string, float>& parameter)
{
	{
		auto iter = parameter.find("aberrationAmount");
		if (iter != parameter.end())
			_data.aberrationAmount = (*iter).second / 100.0f;
	}
	{
		auto iter = parameter.find("maxSamples");
		if (iter != parameter.end())
			_data.maxSamples = (unsigned int)(*iter).second;
	}
	{
		auto iter = parameter.find("shift[0].x");
		if (iter != parameter.end())
			_data.shift[0].x = (*iter).second;
	}
	{
		auto iter = parameter.find("shift[0].y");
		if (iter != parameter.end())
			_data.shift[0].y = (*iter).second;
	}
	{
		auto iter = parameter.find("shift[0].z");
		if (iter != parameter.end())
			_data.shift[0].z = (*iter).second;
	}
	{
		auto iter = parameter.find("shift[1].x");
		if (iter != parameter.end())
			_data.shift[1].x = (*iter).second;
	}
	{
		auto iter = parameter.find("shift[1].y");
		if (iter != parameter.end())
			_data.shift[1].y = (*iter).second;
	}
	{
		auto iter = parameter.find("shift[1].z");
		if (iter != parameter.end())
			_data.shift[1].z = (*iter).second;
	}
	{
		auto iter = parameter.find("shift[2].x");
		if (iter != parameter.end())
			_data.shift[2].x = (*iter).second;
	}
	{
		auto iter = parameter.find("shift[2].y");
		if (iter != parameter.end())
			_data.shift[2].y = (*iter).second;
	}
	{
		auto iter = parameter.find("shift[2].z");
		if (iter != parameter.end())
			_data.shift[2].z = (*iter).second;
	}
}

// 定数バッファの更新
void ChromaticAberration::UpdateConstantBuffer(ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer)
{
	immediateContext->UpdateSubresource(constantBuffer, 0, 0, &_data, 0, 0);
}
