#include "FXAA.h"

#include <imgui.h>

// デバッグGui描画
void FXAA::DrawGui()
{
	bool flag = _data.useFlag != 0.0f;
	ImGui::Checkbox("use", &flag);
	_data.useFlag = flag ? 1.0f : 0.0f;
	ImGui::DragFloat("subpix", &_data.subpix, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("edgeThreshold", &_data.edgeThreshold, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("edgeThresholdMin", &_data.edgeThresholdMin, 0.01f, 0.0f, 1.0f);
}

// 現在のデータの取得
std::unordered_map<std::string, float> FXAA::GetCurrentData()
{
	std::unordered_map<std::string, float> parameter;
	parameter["subpix"] = _data.subpix;
	parameter["edgeThreshold"] = _data.edgeThreshold;
	parameter["edgeThresholdMin"] = _data.edgeThresholdMin;
	return parameter;
}

// データのセット
void FXAA::SetData(std::unordered_map<std::string, float>& parameter)
{
	{
		auto iter = parameter.find("subpix");
		if (iter != parameter.end())
			_data.subpix = (*iter).second;
	}
	{
		auto iter = parameter.find("edgeThreshold");
		if (iter != parameter.end())
			_data.edgeThreshold = (*iter).second;
	}
	{
		auto iter = parameter.find("edgeThresholdMin");
		if (iter != parameter.end())
			_data.edgeThresholdMin = (*iter).second;
	}
}

// 定数バッファの更新
void FXAA::UpdateConstantBuffer(ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer)
{
	immediateContext->UpdateSubresource(constantBuffer, 0, 0, &_data, 0, 0);
}
