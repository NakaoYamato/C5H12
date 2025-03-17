#include "GaussianFilter.h"
#include <imgui.h>

#include "../../Library/Graphics/Graphics.h"

void GaussianFilter::Update(float elapsedTime)
{
	_data.textureSize.x = Graphics::Instance().GetScreenWidth();
	_data.textureSize.y = Graphics::Instance().GetScreenHeight();
}

// デバッグGui描画
void GaussianFilter::DrawGui()
{
	if (ImGui::Begin(u8"ガウスフィルター"))
	{
		if (ImGui::Button("reset"))
			ClearData();
		ImGui::SliderInt("kernel", &_data.kernelSize, 1, _KERNEL_MAX);
		ImGui::SliderFloat("sigma", &_data.sigma, 1.0f, 10.0f);
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

// 定数バッファの更新
void GaussianFilter::UpdateConstantBuffer(ID3D11DeviceContext* immediateContext, 
	ID3D11Buffer* constantBuffer)
{
	// フィルターの計算
	CalcGaussianFilterConstant(_constant, _data);

	immediateContext->UpdateSubresource(constantBuffer, 0, 0, &_constant, 0, 0);
}

// フィルターの計算
void GaussianFilter::CalcGaussianFilterConstant(Constants& constant, const Datas data)
{
	// 偶数の場合は奇数に直す
	int kernelSize = data.kernelSize;
	if (kernelSize % 2 == 0) kernelSize++;
	constant.kernelSize = static_cast<float>(kernelSize);
	constant.texcel.x = 1.0f / data.textureSize.x;
	constant.texcel.y = 1.0f / data.textureSize.y;

	// 重みを算出
	float sum = 0.0f;	// 合計値
	int id = 0;
	for (int y = -kernelSize / 2; y <= kernelSize / 2; ++y)
	{
		for (int x = -kernelSize / 2; x <= kernelSize / 2; ++x)
		{
			constant.weights[id].x = (float)x;
			constant.weights[id].y = (float)y;
			constant.weights[id].z = (float)exp(-(x * x + y * y) / (2.0f * data.sigma * data.sigma)) / (2.0f * DirectX::XM_PI * data.sigma);
			sum += constant.weights[id].z;
			id++;
		}
	}

	// 平均化
	for (int i = 0; i < kernelSize * kernelSize; i++)
	{
		constant.weights[i].z /= sum;
	}
}

std::unordered_map<std::string, float> GaussianFilter::GetCurrentData()
{
	std::unordered_map<std::string, float> parameter;
	parameter["kernelSize"] = (float)_data.kernelSize;
	parameter["sigma"] = _data.sigma;
	return parameter;
}

void GaussianFilter::SetData(std::unordered_map<std::string, float>& parameter)
{
	{
		auto iter = parameter.find("kernelSize");
		if (iter != parameter.end())
			_data.kernelSize = (int)(*iter).second;
	}
	{
		auto iter = parameter.find("sigma");
		if (iter != parameter.end())
			_data.sigma = (*iter).second;
	}
}
