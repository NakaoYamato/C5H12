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
		float chromaticAberrationAmount = data.chromaticAberrationAmount * 100.0f;
		ImGui::SliderFloat("amount", &chromaticAberrationAmount, 0.0f, +1.0f);
		data.chromaticAberrationAmount = chromaticAberrationAmount * 0.01f;
		int chromaticAberrationMaxSamples = data.chromaticAberrationMaxSamples;
		ImGui::SliderInt("max sample", &chromaticAberrationMaxSamples, 0, 100);
		data.chromaticAberrationMaxSamples = chromaticAberrationMaxSamples;
		ImGui::SliderFloat3("shift 0", &data.chromaticAberrationShift[0].x, 0.0f, +1.0f);
		ImGui::SliderFloat3("shift 1", &data.chromaticAberrationShift[1].x, 0.0f, +1.0f);
		ImGui::SliderFloat3("shift 2", &data.chromaticAberrationShift[2].x, 0.0f, +1.0f);
	}
	ImGui::End();
}

// 定数バッファの更新
void ChromaticAberration::UpdateConstantBuffer(ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer)
{
	immediateContext->UpdateSubresource(constantBuffer, 0, 0, &data, 0, 0);
}
